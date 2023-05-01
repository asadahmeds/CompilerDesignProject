
int a;

long b, c; // globals

/* function to calculate
sum of two numbers */
int calc_sum(float x, float y)
{
	int sum;

	sum = x + y;
	
	return sum /* x+y */;
}

int is_digit(char ch)
{
    return ch >= 0 && ch <= 9;
}

int main()
{
	int a, b;

	a = calc_sum(5, 6.5);
	b = is_digit('x');

	return 0;
}