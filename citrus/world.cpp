#include "world.h"

namespace citrus {
	namespace dynamics {
		void citrus::dynamics::world::addBody(rigidBody * body) {
			std::lock_guard<std::mutex> lock(_toBeAddedMut);
			_toBeAdded.insert(body);
		}
		void world::removeBody(rigidBody * body) {
			std::lock_guard<std::mutex> lock(_toBeAddedMut);
			if(_toBeAdded.erase(body) == 0) {
				std::lock_guard<std::mutex> lock2(_toBeRemovedMut);
				_toBeRemoved.insert(body);
			}
		}
		void world::flushAddRemove() {
			std::lock_guard<std::mutex> lock0(_toBeAddedMut), lock1(_toBeRemovedMut);
			for(rigidBody* body : _toBeAdded) {
				_bodies.insert(body);
				_world->addRigidBody(body->ptr());
			}
			_toBeAdded.clear();
			for(rigidBody* body : _toBeRemoved) {
				_bodies.erase(body);
				_world->removeRigidBody(body->ptr());
			}
			_toBeRemoved.clear();
		}
		void world::step() {
			_world->stepSimulation(1.0 / 60.0, 1, 1.0 / 60.0);

			std::lock_guard<std::mutex> lock(_toBeUpdatedMut);
			for(rigidBody* body : _bodies) {
				_toBeUpdated.push_back(bodyUpdateInfo{
					body,
					util::btToGlm(body->ptr()->getWorldTransform().getOrigin()),
					util::btToGlm(body->ptr()->getOrientation())
					});
			}
		}
		void world::updateBodyInfo() {
			std::lock_guard<std::mutex> lock(_toBeUpdatedMut);
			for(bodyUpdateInfo info : _toBeUpdated) {
				info.body->_position = info.newPosition;
				info.body->_orientation = info.newOrientation;
			}
		}
		world::world() :
			_collisionConfiguration(new btDefaultCollisionConfiguration()),
			_dispatcher(new btCollisionDispatcher(_collisionConfiguration.get())),
			_broadphaseInterface(new btDbvtBroadphase()),
			_constraintSolver(new btSequentialImpulseConstraintSolver()),
			_world(new btDiscreteDynamicsWorld(_dispatcher.get(), _broadphaseInterface.get(), _constraintSolver.get(), _collisionConfiguration.get())) {
			_world->setGravity(btVector3(0.0, -9.81, 0.0));
		}
	}
}
