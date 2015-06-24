package com.game.hanzi.wormsplat;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

public class Physics {

  	Vector gravity = new Vector(0,2.0);
	Constraint[] constraints = new Constraint[40];
  	Point[] points = new Point[20];
	int total_points = 0;
	int total_constraints = 0;
  	Paint p = new Paint();
	
  	public Physics()
  	{
  		make(10, 50, 10, 10);
  	}
  	
  	
	public void simulate()
	{
        double steps = 15;
        double delta = 1/steps;
        int j,i,k,c;
        Point point;
        
        c = total_constraints;
        k = total_points;
        for(j=0; j<steps; j++){
            for(i=0; i<constraints.length; i++){
                constraints[i].resolve();
            }

            for(i=0; i<points.length; i++){
                point = points[i];
                point.accelerate(gravity);
                point.simulate(delta);
            }
        }
    }

	public void make(int x, int y, int length, int segments){
        
		int i,j,k;
		j = 0;
		k = 0;
		Point top = new Point(x, y, true);
        Point bottom = new Point(x, y+length, true);
        
        points[j] = top;
        j++;
        points[j] = bottom;
        j++;
        
        
        for( i=0; i<segments; i++){
            Point new_top = new Point(x+i*length, y, false);
            Point new_bottom = new Point(x+i*length, y+length, false);
            
            points[j] = new_top;
            j++;
            points[j] = new_bottom;
            j++;
            
            constraints[k] = new Constraint(top, new_top);
            k++;
            constraints[k] = new Constraint(bottom, new_bottom);
            k++;
            constraints[k] = new Constraint(new_top, new_bottom);
            k++;
            constraints[k] = new Constraint(top, new_bottom);
            k++;
            top = new_top;
            bottom = new_bottom;
        }
        this.total_constraints = k;
        this.total_points = j;
	}

	public void draw(Canvas c)
	{
		int i, j;
		float x1, y1, x2, y2;
		
		
		for(i=0;i<total_constraints;i++)
		{
			x1 = (float) constraints[i].p1.position.x;
			y1 = (float) constraints[i].p1.position.y;
			x2 = (float) constraints[i].p2.position.x;
			y2 = (float) constraints[i].p2.position.y;
			
			p.setColor(Color.WHITE);
	        p.setStrokeWidth(0);
			// draw points
			c.drawCircle((int)x1, (int)y1, 3, p);
			c.drawCircle((int)x2, (int)y2, 3, p);
			// draw constraint
			c.drawLine(x1, y1, x2, y2, p);
			
			
		}
		
	}
	
}
