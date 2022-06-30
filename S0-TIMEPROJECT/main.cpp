#include <iostream>

using namespace std;

int random()
{
	srand((unsigned)time(NULL));
	int number = (100 * rand() / (100 + 1));
	return number;
}

void RandNumberMain()
{
	int x = random();
	while (x == 6) {
		x = random();
	}
}

int main()
{
	RandNumberMain();

	return 0;
}