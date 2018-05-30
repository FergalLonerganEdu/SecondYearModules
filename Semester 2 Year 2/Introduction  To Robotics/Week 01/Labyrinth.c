#define ROTATIONAL_DISTANCE 150
#define ANGLE_ROTATION 240

void move(int speed, int distance)
{
	int target = ((distance/ROTATIONAL_DISTANCE)*360);
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	while(nMotorEncoder[motorB] <= target)
	{
		motor[motorB] = speed;
		motor[motorC] = speed;
	}
	motor[motorB] = 0;
	motor[motorC] = 0;
		nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
}

void turnLeft(int speed, int angle)
{
	int target = ((angle*360)/ANGLE_ROTATION);
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	while(nMotorEncoder[motorB] >= -target)
	{
		motor[motorB] = -speed;
		motor[motorC] = speed;
	}
	motor[motorB] = 0;
	motor[motorC] = 0;
		nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
}


void turnRight(int speed, int angle)
{
	int target = ((angle*360)/ANGLE_ROTATION);
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	while(nMotorEncoder[motorC] >= -target)
{
		motor[motorB] = speed;
		motor[motorC] = -speed;
	}
	motor[motorB] = 0;
	motor[motorC] = 0;
		nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
}


task main()
{
	move(100, 657);
	turnLeft(100, 90);
	move(100, 936);
	turnRight(100,90);
	move(100, 412);
	turnRight(100, 90);
	move(100, 468);
}
