#include <vector>
#include <map>
using namespace std;
class Globals {
public:
	class Drawing {
	public:
		string mode;
		string shadding;
		vector<float> background;
	};

	class Culling {
	public:
		string face;
		string order;
	};

	class Lighting {
	public:
		bool doublesided, local, enabled;
		vector<float> ambient;
	};

	Drawing drawing;
	Culling culling;
	Lighting lighting;
};

class Cameras {
public:
	class Camera {
		;
	};
	class Perspective: public Camera {
	public:
		float near, far, angle;
		vector<float> pos, target;

	};
	class Ortho: public Camera {
	public:
		char direction;
		float near, far, left, right, top, bottom;
	};
	string initial;
	map<string, Camera> camera;
};

class Lights {
public:
	class Light {
	public:
		string type;
		bool enabled, marker;
		vector<float> pos, ambient, diffuse, specular;
	};
	class Omni: public Light {

	};
	class Spot: public Light {
	public:
		vector<float> pos, target;
		float angle, exponent;
	};
	map<string, Light> light;
};

class Textures {
public:
	class Texture {
	public:
		string file;
		float texlength_s, texlength_t;
	};
	map<string, Texture> texture;
};

class Appearence {
public:
	CGFappearance appearence;
	string textureref;
};

class Graph {
public:
	class Node {
	public:
		class Rectangle {
		public:
			vector<float> xy1, xy2;
		};
		class Triangle {
		public:
			vector<float> xyz1, xyz2, xyz3;
		};
		class Cylinder {
		public:
			float base, top, height;
			int slices, stacks;
		};
		class Sphere {
		public:
			float radius;
			int slices, stacks;
		};
		class Torus {
		public:
			float inner, outer;
			int slices, loops;
		};
		float matrix[16];
		string appearencerefID;
		vector<Rectangle> rectangle;
		vector<Triangle> triangle;
		vector<Cylinder> cylinder;
		vector<Sphere> sphere;
		vector<Torus> torus;
		vector<Node*> descendant;
		Node() {
			for (int i = 0; i < 16; ++i)
				matrix[i] = 0;
		}
	};
	map<string, Node> nodes;
	string rootid;
};

