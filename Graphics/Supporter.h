#pragma once
class Supporter
{
private:
	double x, y;
	int health = 100;
	int ammoHolding = 0;
	int medicalHolding = 0;


public:
	Supporter(double xx, double yy);
	void setX(double sx) { x = sx; }
	void sety(double sy) { y = sy; }
	void setHealth(int h) { health = h; }
	void setAmmo(int ah) { ammoHolding = ah; }
	void setMedical(int mh) { medicalHolding= mh; }
	double getX() { return x; }
	double getY() { return y; }
	int getHealth() { return health; }
};

