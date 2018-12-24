#include <dynamics/physics/world.h>
#include <util/util.h>
#include <dynamics/physics/collisionShape.h>

#include <iostream>

namespace citrus {
	namespace dynamics {
		void citrus::dynamics::world::addBody(rigidBody * body) {
			_world->addRigidBody(body->_body);
			_bodies.insert(body);
		}
		void world::removeBody(rigidBody * body) {
			_world->removeRigidBody(body->_body);
			_bodies.erase(body);
		}
		void world::step() {
			/*for(rigidBody* body : _bodies) {
				if(body->dynamic) {
					body->_body->setMassProps(body->mass, btVector3(1.0, 1.0, 1.0));
				} else {
					body->_body->setMassProps(0.0, btVector3(1.0, 1.0, 1.0));
				}
			}*/
			_world->stepSimulation(1.0 / 60.0, 1, 1.0 / 60.0);
		}
		world::world() :
			_collisionConfiguration(new btDefaultCollisionConfiguration()),
			_dispatcher(new btCollisionDispatcher(_collisionConfiguration.get())),
			_broadphaseInterface(new btDbvtBroadphase()),
			_constraintSolver(new btSequentialImpulseConstraintSolver()),
			_world(new btDiscreteDynamicsWorld(_dispatcher.get(), _broadphaseInterface.get(), _constraintSolver.get(), _collisionConfiguration.get())) {
			_world->setGravity(btVector3(0.0, -0.0981, 0.0));

			collisionShape* sh = new collisionShape(0.5f, 0.5f, 0.5f);
			rigidBody* bd = new rigidBody(sh, this);
			bd->dynamic = false;
			bd->setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
			bd->_body->setMassProps(0.0, btVector3(0.0, 0.0, 0.0));
		}
	}
}