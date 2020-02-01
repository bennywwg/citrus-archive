{
	"Elements": [
		{
		  "Name": "shapeEle",
		  "Init": {
		    "type": 4,
			"state": {"x": 0, "y": 0, "z": 0},
			"ext": "C:/Users/benny/Build/citrus/res/unused/transitionDoor_collision.dae",
			"verts": [],
			"indices": []
		  }
		},
		{
		  "Name": "collisionEle",
		  "Init": { }
		},
        {
          "Name": "modelEle",
          "Init": [ 1, 5, 5, 4 ]
        },
        {
          "Name": "doorController",
          "Init": {}
        }
    ],
  "Entities": [
    {
      "ID": 2,
      "Name": "barrier",
      "Transform": {
        "Position": {
          "x": 0,
          "y": 0,
          "z": 0
        },
        "Orientation": {
          "x": 0,
          "y": 0,
          "z": 0,
          "w": 1
        }
      },
      "Parent": 0,
      "Elements": [
        {
          "Name": "shapeEle",
          "Init": {
            "type": 2,
            "state": {
              "x": 2,
              "y": 2,
              "z": 0.1
            },
            "verts": [],
            "indices": []
          }
        },
        {
          "Name": "collisionEle",
          "Init": {}
        }
      ]
    },
    {
      "ID": 3,
      "Name": "barrierSensor",
      "Transform": {
        "Position": {
          "x": 0,
          "y": 0,
          "z": 0
        },
        "Orientation": {
          "x": 0,
          "y": 0,
          "z": 0,
          "w": 1
        }
      },
      "Parent": 0,
      "Elements": [
        {
          "Name": "shapeEle",
          "Init": {
            "type": 2,
            "state": {
              "x": 1,
              "y": 1,
              "z": 0.2
            },
            "verts": [],
            "indices": []
          }
        },
        {
          "Name": "sensorEle",
          "Init": {}
        }
      ]
    },
    {
      "ID": 4,
      "Name": "frontSensor",
      "Transform": {
        "Position": {
          "x": 0,
          "y": 0,
          "z": 1
        },
        "Orientation": {
          "x": 0,
          "y": 0,
          "z": 0,
          "w": 1
        }
      },
      "Parent": 0,
      "Elements": [
        {
          "Name": "shapeEle",
          "Init": {
            "type": 2,
            "state": {
              "x": 1,
              "y": 1,
              "z": 1
            },
            "verts": [],
            "indices": []
          }
        },
        {
          "Name": "sensorEle",
          "Init": {}
        }
      ]
    },
    {
      "ID": 5,
      "Name": "backSensor",
      "Transform": {
        "Position": {
          "x": 0,
          "y": 0,
          "z": -1
        },
        "Orientation": {
          "x": 0,
          "y": 0,
          "z": 0,
          "w": 1
        }
      },
      "Parent": 0,
      "Elements": [
        {
          "Name": "shapeEle",
          "Init": {
            "type": 2,
            "state": {
              "x": 1,
              "y": 1,
              "z": 1
            },
            "verts": [],
            "indices": []
          }
        },
        {
          "Name": "sensorEle",
          "Init": {}
        }
      ]
    }
  ]
}