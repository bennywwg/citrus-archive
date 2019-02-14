#include <citrus/dynamics/world.h>
#include <citrus/util.h>
#include <citrus/dynamics/collisionShape.h>
#include <citrus/dynamics/sensor.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace citrus {
	namespace dynamics {
		void citrus::dynamics::world::addBody(rigidBody * body) {
			_world->addRigidBody(body->_body);
			_world->computeOverlappingPairs();
			_bodies.insert(body);
		}
		void world::removeBody(rigidBody * body) {
			_world->removeRigidBody(body->_body);
			_world->computeOverlappingPairs();
			_bodies.erase(body);
		}
		void world::addSensor(sensor * se) {
			_world->addCollisionObject(se->ptr());
			_world->computeOverlappingPairs();
			_sensors.insert(se);
		}
		void world::removeSensor(sensor * se) {
			_world->removeCollisionObject(se->ptr());
			_world->computeOverlappingPairs();
			_sensors.erase(se);
		}
		void world::step() {
			for(rigidBody* body : _bodies) {
				if(body->dynamic) {
					body->_body->setMassProps(body->mass, btVector3(1.0, 1.0, 1.0));
				} else {
					body->_body->setMassProps(0.0, btVector3(1.0, 1.0, 1.0));
				}
			}
			_world->stepSimulation(1.0 / 100.0, 1, 1.0 / 100.0);
		}
		world::world() :
			_collisionConfiguration(new btDefaultCollisionConfiguration()),
			_dispatcher(new btCollisionDispatcher(_collisionConfiguration.get())),
			_broadphaseInterface(new btDbvtBroadphase()),
			_constraintSolver(new btSequentialImpulseConstraintSolver()),
			_world(new btDiscreteDynamicsWorld(_dispatcher.get(), _broadphaseInterface.get(), _constraintSolver.get(), _collisionConfiguration.get())) {
			_world->setGravity(btVector3(0.0, -9.81, 0.0));
			_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		}
	}
}