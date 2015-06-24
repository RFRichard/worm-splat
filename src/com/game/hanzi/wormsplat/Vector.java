package com.game.hanzi.wormsplat;


import java.lang.Math;


public class Vector {

	public double x, y;
	
	public Vector(double x, double y)
	{
		this.x = x;
		this.y = y;
	}
	
	static public void isub(Vector vout, Vector vin)
	{
	        vout.x -= vin.x;
	        vout.y -= vin.y;
	}

	static public Vector sub(Vector vout, Vector vin)
	{
	        vout.x -= vin.x;
	        vout.y -= vin.y;
	        return vout;
	}

	static public void iadd(Vector vout, Vector vin)
	{
	        vout.x += vin.x;
	        vout.y += vin.y;
	}

	static public Vector add(Vector vout, Vector vin)
	{
	        vout.x += vin.x;
	        vout.y += vin.y;
	        return vout;
	}

	static public Vector mul(Vector vout, double scalar)
	{
	        vout.x *= scalar;
	        vout.y *= scalar;
	        return vout;
	}

	
	public Vector vdiv(Vector vout, double scalar)
	{
	        vout.x /= scalar;
	        vout.y /= scalar;
	        return vout;
	}

	static public Vector normalized(Vector vin)
	{
	  double x = vin.x;
	  double y = vin.y;
	  double length = Math.sqrt(x*x + y*y);
	  
	  if(length > 0){
	      vin.x /= length;
	      vin.y /= length;
	  }
	  else{
	      vin.x = 0;
	      vin.y = 0;
	  }

	  return vin;
	  
	}

	static public void normalize(Vector vin)
	{
	  double x = vin.x;
	  double y = vin.y;
	  double length = Math.sqrt(x*x + y*y);
	  
	  if(length > 0){
	      vin.x /= length;
	      vin.y /= length;
	  }
	    
	}

	static public double length(Vector vin)
	{
	  double x = vin.x;
	  double y = vin.y;
	  double length = Math.sqrt(x*x + y*y);

	  return length;
	}

	static double distance(Vector v1, Vector v2)
	{
	  double x = v1.x - v2.x;
	  double y = v1.y - v2.y;
	  double dist = Math.sqrt(x*x + y*y);

	  return dist;
	}

	
	static void zero(Vector vin)
	{ vin.x = 0;
	  vin.y = 0;
	}
	
	
}
