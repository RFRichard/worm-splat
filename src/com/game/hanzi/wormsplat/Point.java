package com.game.hanzi.wormsplat;

public class Point {

	public Vector position;
	public Vector previous;
	public Vector acceleration;
	public boolean fixed;
	public int id;
	
	public Point(Vector pos, Vector pre, Vector acc, boolean fixed)
	{
		position = pos;
		previous = pre;
		acceleration = acc;
		this.fixed = fixed;
	}
	      
	public Point(int x, int y, boolean fixed)
	{
		position = new Vector(x,y);
		previous =  new Vector(0,0);
		acceleration =  new Vector(0,0);
		this.fixed = fixed;
	}
	
	public Point()
	{
		position = new Vector(0,0);
		previous =  new Vector(0,0);
		acceleration =  new Vector(0,0);
		fixed = false;
	}
	
	public void accelerate(Vector v)
	{
		this.acceleration = Vector.add(this.acceleration, v);
	}
	
	public void correct(Vector v)
	{
		this.position = Vector.add(this.acceleration, v);
	}
			
	public void simulate(double delta)
	{
			
	}

}
