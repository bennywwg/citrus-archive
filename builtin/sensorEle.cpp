#include "sensorEle.h"

namespace citrus {
	class customCB : public btCollisionWorld::ContactResultCallback {
	public:
		vector<const btCollisionObject*> objs;
		btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
			objs.push_back(colObj1Wrap->getCollisionObject());
			return 0.0f;
		}
	};

	void sensorEle::action() {
		_body->setWorldTransform(glmToBt(ent().getGlobalTrans()));
	}

	bool sensorEle::touchingAny() const {
		return false;
	}

	int sensorEle::numTouching() const {
		auto pairs = _body->getOverlappingPairs();
		
		int num = 0;
		for (int i = 0; i < pairs.size(); i++) {
			customCB cb;
			_world->_world->contactPairTest(_body, pairs.at(i), cb);
			for (auto ptr : cb.objs) {
				num++;
				//std::cout << "touching: " << (char const*)ptr->getUserPointer() << "\n";
			}
		}
		//std::cout << "\n";
		return num;
	}

	std::vector<eleRef<shapeEle>> sensorEle::touching() const {
		/*for (int i = 0; i < _body->getNumOverlappingObjects(); i++) {
			auto obj = _body->getOverlappingObject(i);

		}*/
		return { };
	}

	sensorEle::sensorEle(entRef const& ent, manager& man, void* usr) :
		element(ent, man, usr, typeid(sensorEle)),
		_world((world*)usr)
	{
		eleRef<shapeEle> const& myShape = ent.getEle<shapeEle>();
		if (!myShape.id()) throw std::runtime_error("sensorEle needs shapeEle");

		// make shape in world
		_shape = new worldShape(myShape->getShape());

		// make body
		_body = new btGhostObject();
		_body->setCollisionShape(_shape->wdshape);
		_body->setUserPointer((void*)"sensorEle");
		_body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

		_world->addSensor(_body);
	}

	sensorEle::~sensorEle() {
		_world->removeSensor(_body);
		delete _body;
		delete _shape;
	}
}