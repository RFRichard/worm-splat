package com.game.hanzi.wormsplat;

import java.util.ArrayList;



public class GameLevels {

	private int currentLevel = 0;
	private float currentLevelFactor = 0;
	private int currentMinTime = 20;
	private int currentEnemyCount = 50;
	private float currentGameSpeed = 1;
	private int highScore = 0;
	
	private int[] lvlMinTime = new int[30];
	private ArrayList<LevelData> levelData;
    
	private class LevelData{
    	
		private int currentLevel = 1;
    	private float currentLevelFactor = 0;
    	private int currentMinTime = 20;
    	private int currentEnemyCount = 50;
    	private float currentGameSpeed = 1;
    	
    	public LevelData(int levelIndex, float levelFactor, int levelMinTime, int levelEnemyCount, float levelGameSpeed){
    		currentLevel = levelIndex;
    		currentLevelFactor = levelFactor;
    		currentMinTime = levelMinTime;
    		currentEnemyCount = levelEnemyCount;
    		currentGameSpeed  = levelGameSpeed;
    	}
    	
    	public int getCurrentLevel() {
			return currentLevel;
		}
		public void setCurrentLevel(int currentLevel) {
			this.currentLevel = currentLevel;
		}
		public float getCurrentLevelFactor() {
			return currentLevelFactor;
		}
		public void setCurrentLevelFactor(float currentLevelFactor) {
			this.currentLevelFactor = currentLevelFactor;
		}
		public int getCurrentMinTime() {
			return currentMinTime;
		}
		public void setCurrentMinTime(int currentMinTime) {
			this.currentMinTime = currentMinTime;
		}
		public int getCurrentEnemyCount() {
			return currentEnemyCount;
		}
		public void setCurrentEnemyCount(int currentEnemyCount) {
			this.currentEnemyCount = currentEnemyCount;
		}
		public float getCurrentGameSpeed() {
			return currentGameSpeed;
		}
		public void setCurrentGameSpeed(float currentGameSpeed) {
			this.currentGameSpeed = currentGameSpeed;
		}
    	
    }
	
	public GameLevels(){
		this.currentLevel = 1;
		// set levels
		levelData = new ArrayList<LevelData>();
		levelData.add(new LevelData(1,1,25000,30,1));
		levelData.add(new LevelData(2,1,15000,40,1));
		levelData.add(new LevelData(3,1,12000,50,1));
		levelData.add(new LevelData(4,1,12000,60,1));
		levelData.add(new LevelData(5,1,12000,50,1));
		levelData.add(new LevelData(6,1,11000,50,1));
		levelData.add(new LevelData(7,1,11000,50,1));
		levelData.add(new LevelData(8,1,10000,50,1));
		levelData.add(new LevelData(9,1, 9000,50,1));
		levelData.add(new LevelData(10,1,9000,50,1));
		levelData.add(new LevelData(11,1,8000,50,1));
		levelData.add(new LevelData(12,1,7000,40,1));
		// set current level
		
		this.setCurrentLevel(this.currentLevel);		
	}
	
		
	public void setCurrentLevel(int currentLevel) {
		this.currentLevel = currentLevel;
		int index = this.currentLevel - 1;
		if(currentLevel<levelData.size()){
			this.currentLevel = currentLevel;
			this.currentLevelFactor = levelData.get(index).getCurrentLevelFactor();
			this.currentMinTime = levelData.get(index).getCurrentMinTime();
			this.currentEnemyCount = levelData.get(index).getCurrentEnemyCount();
			this.currentGameSpeed  = levelData.get(index).getCurrentGameSpeed();
		}
	}
	
	public int getCurrentLevel() {
		return currentLevel;
	}

	public int getCurrentMinTime() {
		return currentMinTime;
	}

	public int getHighScore() {
		return highScore;
	}

	public void increaseLevel(){
		currentLevel++;
		this.setCurrentLevel(currentLevel);
	}

	public void setCurrentEnemyCount(int currentEnemyCount) {
		this.currentEnemyCount = currentEnemyCount;
	}

	public int getCurrentEnemyCount() {
		return currentEnemyCount;
	}

	public void setCurrentGameSpeed(int currentGameSpeed) {
		this.currentGameSpeed = currentGameSpeed;
	}

	public float getCurrentGameSpeed() {
		return currentGameSpeed;
	}
	
}
