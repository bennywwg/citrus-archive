{
	"Entities": [
		{
			"Name": "door0",
			"ID": 420,
			"Parent": 0,
			"Transform": {
				"Position": {"x": -3.52551, "y": 0.475688, "z": 0},
				"Orientation": {"x": 0, "y": 0.70710678118, "z": 0, "w": 0.70710678118}
			},
			"Elements": []
		},
		{
			"Name": "door1",
			"ID": 423,
			"Parent": 0,
			"Transform": {
				"Position": {"x": -7.91611, "y": 8.52643, "z": -7.51044},
				"Orientation": {"x": 0, "y": 0.819152, "z": 0, "w": 0.573576}
			},
			"Elements": []
		}
	],
	"Elements": [
		{
			"Name": "roomController",
			"Init": [
				{
					"name": "door0",
					"connectedLevelTree": "level.cts",
					"connectedName": "door1"
				},
				{
					"name": "door1",
					"connectedLevelTree": "level.cts",
					"connectedName": "door0"
				}
			]
		},
		{
			"Name": "shapeEle",
			"Init": {
				"type": 4,
				"state": {"x": 1.2, "y": 0.25, "z": 0},
				"ext": "C:/Users/benny/Build/citrus/res/meshes/level.dae",
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
			"Init": [0, 2, 2, 4]
		}
	]
}