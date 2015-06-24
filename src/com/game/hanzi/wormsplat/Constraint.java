package com.game.hanzi.wormsplat;

public class Constraint {
	
	Point p1;
	Point p2;
	double target;
	
	public Constraint(Point p1, Point p2)
	{
		this.p1 = p1;
		this.p2 = p2;
		
	}

	public Constraint()
	{
		p1 = new Point();
		p2 = new Point();
	}
	
	public void resolve()
	{
		 Vector pos1 = this.p1.position;
         Vector pos2 = this.p2.position;
         Vector direction = Vector.sub(pos2, pos1);
         double length = Vector.length(direction);
         double factor = (length-this.target)/(length*2.1);
         Vector correction = Vector.mul(direction, factor);

         this.p1.correct(correction);
         Vector.mul(correction, -1);
         this.p2.correct(correction);
		
	}
	
}
