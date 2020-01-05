{
  "Entities": [
    {
  	  "ID": 1,
  	  "Name": "door",
  	  "Transform": {
	    "Position": {"x": 1, "y": 4, "z": 0},
		"Orientation": {"x": 0, "y": 0, "z": 0, "w": 1}
	  },
      "Parent": 0,
	  "Elements": [
	    {
		  "Name": "modelEle",
		  "Init": [1, 5, 5, 4]
		},
		{
		  "Name": "shapeEle",
		  "Init": {
					"type": 2,
					"state": {"x": 1, "y": 1, "z": 1},
					"verts": [],
					"indices": []
				}
		},
		{
		  "Name": "collisionEle",
		  "Init": { }
		}
	  ]
    }
  ]
}