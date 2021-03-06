using System;
using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

//MANAGER
public class GameManager : Singleton<GameManager>
{
	//FIELDS
	Image scanner;
	
	private int distanceBlockCount = 0;
	private int distanceBlock = -1;
	private int tresholdDistanceBlockCount = 20;

	private int count=0; // gewoon om een verschil te zien in de debugger
	
	//PROPERTIES

	
	//METHODS
	void Start()
	{
		
	}

	public void Init(Image scanner) // aangeroepen van SetupSceneScript
	{
		this.scanner = scanner;
	}

	public void onRobotConnected(BLERobot robot)
	{
		Debug.Log ("onRobotConnected("+robot.id+")");
	}

	public void onRobotDisconnected(BLERobot robot)
	{
		Debug.Log ("onRobotDisconnected("+robot.id+")");

		if (scanner)
			scanner.sprite = Resources.Load<Sprite>("Sprites/dist_5");
	}
	
	public void onRSSIUpdate(BLERobot robot, int rssi)
	{
		Debug.Log (count++ + " rssi: " +rssi);


		if (!scanner)
			return;


		if (rssi <= -75) {
			registerDistanceBlock(5);
		}

		if (-70 < rssi && rssi <= -65) {
			registerDistanceBlock(4);

		}

		if (-65 < rssi && rssi <= -60) {
			registerDistanceBlock(3);

		}

		if (-60 < rssi && rssi <= -54) {
			registerDistanceBlock(2);

		}

		if (-54 < rssi) {
			registerDistanceBlock(1);

		}
	}

	public void registerDistanceBlock(int block)
	{
		if(distanceBlock == block){
			distanceBlockCount++;
		}else{
			distanceBlock = block;
			distanceBlockCount = 0;
		}

		// checken of we genoeg keer een afstand in die block geregistreerd hebben
		if(distanceBlockCount >= tresholdDistanceBlockCount) {

			switch(distanceBlock){
			case 5:
				scanner.sprite = Resources.Load<Sprite>("Sprites/dist_5");
				break;

			case 4:
				scanner.sprite = Resources.Load<Sprite>("Sprites/dist_4");
				break;

			case 3:
				scanner.sprite = Resources.Load<Sprite>("Sprites/dist_3");
				break;

			case 2:
				scanner.sprite = Resources.Load<Sprite>("Sprites/dist_2");
				break;

			case 1:
				scanner.sprite = Resources.Load<Sprite>("Sprites/dist_1");
				break;
			}

		}


	}

}
