#include <vector>
#include <iostream>
#include <string>

struct position
{
	float x;
	float y;

	inline position operator*(const double input)
	{
		position temp;
		temp.x = x * input;
		temp.y = y * input;
		return temp;
	}

	inline position operator+(const position& other)
	{
		position temp;
		temp.x = x + other.x;
		temp.y = y + other.y;
		return temp;
	}
};

template<class T>
T linearInterpolation(T& vector0, T& vector1, double time0, double time1, double time)
{
	return vector0 * (1 - (time - time0)/(time1-time0)) + vector1 * ((time-time0)/(time1-time0));
}
template<class T>
T polynomalInterpolation(T& vector0, T& vector1, double time0, double time1, double time2, double time)
{
	//TODO: return vector0 * (1 - (time - time0) / (time1 - time0)) + vector1 * ((time - time0) / (time1 - time0));
}

int main(void)
{
	position pos1;
	pos1.x = 10.0f;
	pos1.y = 10.0f;
	position pos2;
	pos2.x = 15.0f;
	pos2.y = 20.0f;

	int time0 = 100;
	int time1 = 1000;
	int currentTime = 550;

	position interpolatedpos = linearInterpolation(pos1, pos2, time0, time1, currentTime);

	std::cout << interpolatedpos.x << " - " << interpolatedpos.y << std::endl;

	std::string input;
	std::getline(std::cin, input);

	return 0;
}