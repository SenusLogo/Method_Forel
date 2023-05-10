#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <gl/freeglut.h>

using namespace std;

//count images
const int N = 20;

//count features
const int m = 2;

const double tol = 1e-1;


const int X_COORD = 400;
const int Y_COORD = 400;

const float ITER = 0.0001;

int x_off = X_COORD / 2;
int y_off = Y_COORD / 2;

std::vector<double> operator- (const vector<double>& a, const vector<double>& b)
{
	vector<double> result(a.size());

	for (int i(0); i < a.size(); i++)
		result.push_back(a[i] - b[i]);
	return result;
}

double vector_norm(vector<double> a)
{
	double result = 0.;
	for (int i(0); i < a.size(); i++)
		result += pow(a[i], 2);
	return sqrt(result);
}

vector<vector<double>> get_neighbors(vector<double> p, double radius, vector<vector<double>> points)
{
	vector<vector<double>> neighbors;

	for (auto iter : points)
		if (vector_norm(p - iter) < radius)
			neighbors.push_back(iter);

	return neighbors;
}

vector<double> get_centroid(vector<vector<double>> points)
{
	vector<double> result;

	for (int i(0); i < points[0].size(); i++)
	{
		double coordinate = 0.;

		for (int j(0); j < points.size(); j++)
			coordinate += points[j][i];

		result.push_back(coordinate / points.size());
	}

	return result;
}

vector<vector<double>> remove_points(vector<vector<double>> subset, vector<vector<double>> points)
{
	vector<vector<double>> result;

	for (int i(0); i < points.size(); i++)
	{
		for (int j(0); j < subset.size(); j++)
			for (int k(0); k < m; k++)
				if (points[i][k] == subset[j][k])
				{
					j = 0; k = -1;

					if (++i >= points.size())
						return result;
				}

		result.push_back(points[i]);
	}

	return result;
}

vector<vector<double>> cluster(vector<vector<double>> points, double radius)
{
	vector<vector<double>> centroids;

	while (points.size() != 0)
	{
		srand(unsigned int(time(nullptr)));
		vector<double> current_point = points.at(rand() % points.size());
		vector<vector<double>> neighbors = get_neighbors(current_point, radius, points);
		vector<double> centroid = get_centroid(neighbors);

		while (vector_norm(current_point - centroid) > tol)
		{
			std::copy(centroid.begin(), centroid.end(), current_point.begin());
			neighbors = get_neighbors(current_point, radius, points);
			centroid = get_centroid(neighbors);
		}

		points = remove_points(neighbors, points);
		centroids.push_back(current_point);
	}

	return centroids;
}

void drawGrid(float SERIF_OFFSET, float SERIF_DISTANCE)
{
	glBegin(GL_LINES);

	glColor3f(0., 0., 0.);

	glVertex2f(0., Y_COORD / 2);
	glVertex2f(X_COORD, Y_COORD / 2);
	for (int i = X_COORD / 2, p = i; i < X_COORD; i += SERIF_DISTANCE, p -= SERIF_DISTANCE)
	{
		glVertex2f(i, X_COORD / 2);
		glVertex2f(i, X_COORD / 2 + SERIF_OFFSET);

		glVertex2f(p, X_COORD / 2);
		glVertex2f(p, X_COORD / 2 + SERIF_OFFSET);
	}

	glVertex2f(X_COORD / 2, Y_COORD);
	glVertex2f(X_COORD / 2, 0.);
	for (int i = Y_COORD / 2, t = i ; i < Y_COORD; i += SERIF_DISTANCE, t -= SERIF_DISTANCE)
	{
		glVertex2f(X_COORD / 2, i);
		glVertex2f(Y_COORD / 2 + SERIF_OFFSET, i);

		glVertex2f(X_COORD / 2, t);
		glVertex2f(Y_COORD / 2 + SERIF_OFFSET, t);
	}

	glEnd();
}

void drawFunction(vector<vector<double>> X)
{
	glPointSize(5);
	glBegin(GL_POINTS);

		glColor3f(0, 1, 0);

		for (auto iter : X)
			glVertex2f(x_off + iter[0], y_off + iter[1]);

	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	drawGrid(0.5, 10);

	vector<vector<double>> points;

	ifstream data("data.txt");

	string line;
	while (getline(data, line))
	{
		vector<double> number;

		string token;
		stringstream ss;
		double a = 0.;
		size_t start = 0, end;
		while ((end = line.find(' ', start)) != std::string::npos)
		{
			token = line.substr(start, end - start);
			start = end + 1;

			ss << token;
			ss >> a;
			number.push_back(a);
		}

		ss.str(string());
		ss.clear();
		ss << line.substr(start);
		ss >> a;
		number.push_back(a);
		points.push_back(number);
		number.clear();
	}

	vector<vector<double>> result = cluster(points, 20);

	for (auto iter : result)
	{
		for (auto jter : iter)
			cout << jter << " ";
		cout << endl;
	}

	cout << "Method forel select " << result.size() << " cluster" << endl;

	drawFunction(result);

	glutSwapBuffers();

	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(500, 200);
	glutCreateWindow("GLUT_TESTING_APP");

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//пространство координат
	glOrtho(0.0, X_COORD, 0.0, Y_COORD, -1.0, 1.0);

	glutDisplayFunc(display);
	glutMainLoop();

	system("pause");
	return 0;
}
