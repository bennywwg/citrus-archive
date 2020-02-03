{
	"Elements": [
	    {
		  "Name": "shapeEle",
		  "Init": {
		    "type": 5,
			"state": {"x": 0.25, "y": 1.2, "z": 0},
			"verts": [],
			"indices": []
		  }
		},
		{
		  "Name": "rigidEle",
		  "Init": {
		    "kinematic": false,
			"mass": 1.0,
			"friction": 0.0,
			"inertiaMoment": {"x": 0.0, "y": 1.0, "z": 0.0}
		  }
		},
		{
		  "Name": "playerController",
		  "Init": {
			"dist": 1.7,
			"jumpStrength": 2.0,
			"targetSpeed": 5.0,
			"accelFactor": 0.1
		  }
		}
	  ],
  "Entities": [
	{
  	  "ID": 6,
  	  "Name": "playerModel",
  	  "Transform": {
	    "Position": {"x": 0, "y": 0, "z": 0},
		"Orientation": {"x": 0, "y": 0, "z": 0, "w": 1}
	  },
      "Parent": 0,
	  "Elements": [
		{
		  "Name": "modelEle",
		  "Init": [0, 3, 2, 4]
		}
	  ]
    },
	{
		"ID": 2,
		"Name": "legSensor",
		"Transform": {
			"Position": {"x": 0, "y": -0.75, "z": 0},
			"Orientation": {"x": 0, "y": 0, "z": 0, "w": 1}
		},
		"Parent": 0,
		"Elements": [
			{
				"Name": "shapeEle",
				"Init": {
					"type": 1,
					"state": {"x": 0.125, "y": 0, "z": 0},
					"verts": [],
					"indices": []
				}
			},
			{
				"Name": "sensorEle",
				"Init": { }
			}
		]
	},
	{
		"ID": 3,
		"Name": "wallSensor",
		"Transform": {
			"Position": {"x": 0, "y": 0, "z": 0},
			"Orientation": {"x": 0, "y": 0, "z": 0, "w": 1}
		},
		"Parent": 0,
		"Elements": [
			{
				"Name": "shapeEle",
				"Init": {
					"type": 1,
					"state": {"x": 0.125, "y": 0, "z": 0},
					"verts": [],
					"indices": []
				}
			},
			{
				"Name": "sensorEle",
				"Init": { }
			}
		]
	}
  ]
}