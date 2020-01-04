#include "world.h"

namespace citrus {
	class ctBulletDebugDraw : public btIDebugDraw {
	public:
		immediatePass& pass;

		int drawMode;

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
			drawLine(from, to, color, color);
		}
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
			pass.groupings[0].data.push_back(btToGlm(from));
			pass.groupings[0].data.push_back(btToGlm(to));
			pass.groupings[0].altData.push_back(btToGlm(fromColor));
			pass.groupings[0].altData.push_back(btToGlm(toColor));
		}

		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
			// not sure of purpose
		}

		void reportErrorWarning(char const* warningString) {
			std::cout << "ctBulletDebugDraw: " << warningString << "\n";
		}

		void draw3dText(btVector3 const& location, char const* textString) {
			// as if
		}

		void setDebugMode(int val) {
			drawMode = val;
		}

		int getDebugMode() const {
			return drawMode;
		}

		void clearLines() {
			pass.groupings[0].data.clear();
			pass.groupings[0].uvdata.clear();
			pass.groupings[0].altData.clear();
		}

		ctBulletDebugDraw(immediatePass& pass) : pass(pass), drawMode(DBG_MAX_DEBUG_DRAW_MODE){
			pass.groupings.clear();
			grouping gp;
			gp.tr = glm::identity<mat4>();
			gp.color = vec3(1.0f, 1.0f, 1.0f);
			pass.groupings.push_back(gp);
		}
	};

	worldShape::worldShape(shape const& s) : verts(nullptr), indices(nullptr) {
		if (s.type == shapeType::sphere) {
			wdshape = new btSphereShape(s.state.r);
		} else if (s.type == shapeType::box) {
			wdshape = new btBoxShape(glmToBt(s.state));
		} else if (s.type == shapeType::hull) {
			std::vector<btVector3> btVerts;
			btVerts.resize(s.verts.size());
			for (size_t i = 0; i < s.verts.size(); i++) {
				btVerts[i] = btVector3(s.verts[i].x, s.verts[i].y, s.verts[i].z);
			}
			wdshape = new btConvexHullShape((btScalar*)btVerts.data(), btVerts.size(), sizeof(btVector3));
		} else if (s.type == shapeType::triangleMesh) {
			verts = (btScalar*)malloc(sizeof(btScalar) * s.verts.size() * 3);
			indices = (int*)malloc(sizeof(int) * s.indices.size());
			memcpy(indices, s.indices.data(), s.indices.size() * sizeof(int));
			for (int i = 0; i < s.verts.size(); i++) {
				verts[i*3] = s.verts[i].x;
				verts[i*3 + 1] = s.verts[i].y;
				verts[i*3 + 2] = s.verts[i].z;
			}
			btStridingMeshInterface* iface = new btTriangleIndexVertexArray(
				(int)s.indices.size() / 3,
				indices,
				(int)sizeof(int) * 3,
				(int)s.verts.size(),
				verts,
				(int)sizeof(btScalar) * 3
			);
			wdshape = new btBvhTriangleMeshShape(iface, true);
		} else if (s.type == shapeType::capsule) {
			wdshape = new btCapsuleShape(s.state.x, s.state.y);
		} else {
			wdshape = nullptr;
		}
	}
	worldShape::~worldShape() {
		if (verts) free(verts);
		if (indices) free(indices);
		//auto dc = dynamic_cast<btBvhTriangleMeshShape*>(wdshape);
		//if (dc) delete dc->getMeshInterface();
		if (wdshape) delete wdshape;
	}

	void world::addBody(btRigidBody* body, int mask) {
		_world->addRigidBody(body, 1, mask);
		_world->computeOverlappingPairs();
		bodies.insert(body);
	}
	void world::removeBody(btRigidBody* body) {
		_world->removeRigidBody(body);
		_world->computeOverlappingPairs();
		bodies.erase(body);
	}
	void world::addSensor(btGhostObject* se, int mask) {
		_world->addCollisionObject(se, 1, mask);
		_world->computeOverlappingPairs();
		ghosts.insert(se);
	}
	void world::removeSensor(btGhostObject* se) {
		_world->removeCollisionObject(se);
		_world->computeOverlappingPairs();
		ghosts.erase(se);
	}

	void world::addObject(btCollisionObject* object, int mask) {
		_world->addCollisionObject(object, 1, mask);
		_world->computeOverlappingPairs();
		objects.insert(object);
	}
	void world::removeObject(btCollisionObject* object) {
		_world->removeCollisionObject(object);
		_world->computeOverlappingPairs();
		objects.erase(object);
	}

	void world::debugDraw() {
		dbdraw->pass.groupings.clear();
		grouping gp;
		gp.tr = glm::identity<mat4>();
		gp.color = vec3(1.0f, 1.0f, 1.0f);
		dbdraw->pass.groupings.push_back(gp);

		_world->debugDrawWorld();
	}
	void world::step() {
		_world->stepSimulation(0.01f, 10, 0.01f);
	}
	world::world(immediatePass& pass) :
		_collisionConfiguration(new btDefaultCollisionConfiguration()),
		_dispatcher(new btCollisionDispatcher(_collisionConfiguration.get())),
		_broadphaseInterface(new btDbvtBroadphase()),
		_constraintSolver(new btSequentialImpulseConstraintSolver()),
		_world(new btDiscreteDynamicsWorld(_dispatcher.get(), _broadphaseInterface.get(), _constraintSolver.get(), _collisionConfiguration.get()))
	{
		_world->setGravity(btVector3(0.0, -9.81, 0.0));
		_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		dbdraw = new ctBulletDebugDraw(pass);
		_world->setDebugDrawer(dbdraw);
	}
}