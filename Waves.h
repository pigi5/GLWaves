/****************************************************************************

Name: Ford Hash
CSI 4341: Assignment 2: Camera

****************************************************************************/

#ifndef WAVES_H
#define WAVES_H

#include "Shape.h"

class WaveSection {
public:
	Point undisturbedVertices[9] = {
		Point(0.5,0,0.5),
		Point(0,  0,  1),
		Point(0.5,0,  1),
		Point(1,  0,  1),
		Point(1,  0,0.5),
		Point(1,  0,  0),
		Point(0.5,0,  0),
		Point(0,  0,  0),
		Point(0,  0,0.5)
	};
	Point vertices[9];
	Face faces[8] = {
		Face(0,1,2),
		Face(0,2,3),
		Face(0,3,4),
		Face(0,4,5),
		Face(0,5,6),
		Face(0,6,7),
		Face(0,7,8),
		Face(0,8,1)
	};
public:
	WaveSection() {}
	void scaleAndTranslate(Vector scale, Vector translate) {
		for (size_t i = 0; i < 9; i++) {
			undisturbedVertices[i] = scale * undisturbedVertices[i] + translate;
		}
	}
};

class Waves : public Shape {
private:
	unsigned char lodLevels;
	unsigned char levelLayers;
	size_t numSections;
	WaveSection* sections;
    int timeSince = 0;
public:
	Waves(){}
    ~Waves(){};

	void generateMesh() {
		if (sections != NULL) {
			delete[] sections;
		}

		numSections = 4 * (3 * lodLevels + 1) * levelLayers * levelLayers;
		sections = new WaveSection[numSections];
		size_t count = 0;
		unsigned char i, j, k;
		signed char multX, multZ;
		double temp;
		Vector scale;
		Vector disp;
		Vector offset;
		for (multX = -1; multX <= 1; multX += 2) {
			for (multZ = -1; multZ <= 1; multZ += 2) {
				for (i = 0; i < lodLevels; i++) {
					temp = 1.0 / (1 << (lodLevels - i));

					scale[0] = temp * multX;
					scale[2] = temp * multZ;

					offset = scale * levelLayers;
					for (j = 0; j < levelLayers; j++) {
						for (k = 0; k < levelLayers; k++) {
							disp = scale * Vector(k, 0, j);
							if (i == 0) {
								sections[count++].scaleAndTranslate(scale, Vector(scale[0] * k, 0, scale[2] * j));
							}
							sections[count++].scaleAndTranslate(scale, Vector(disp[0] + offset[0], 0, disp[2]));
							sections[count++].scaleAndTranslate(scale, Vector(disp[0] + offset[0], 0, disp[2] + offset[2]));
							sections[count++].scaleAndTranslate(scale, Vector(disp[0], 0, disp[2] + offset[2]));
						}
					}
				}
			}
		}
	}

	void setDetails(unsigned char lods, unsigned char layers) {
		if (levelLayers != layers || lodLevels != lods) {
			levelLayers = layers;
			lodLevels = lods;
			generateMesh();
		}
	}

    void draw() {
        timeSince = glutGet(GLUT_ELAPSED_TIME);

		double offset = timeSince / 1000.0;
		size_t i, j;
		double trigComp;
		Vector waveDirection = Vector(1, 0, 1);
		double amplitude = 0.1;
		for (i = 0; i < numSections; i++) {
			for (j = 0; j < 9; j++) {
				trigComp = dot(waveDirection, sections[i].undisturbedVertices[j]) - sqrt(9.81*waveDirection.length()) * offset;
				sections[i].vertices[j] = sections[i].undisturbedVertices[j] - waveDirection / waveDirection.length() * amplitude * sin(trigComp);
				sections[i].vertices[j][1] = amplitude * cos(trigComp);
			}
		}


        glBegin(GL_TRIANGLES);
		glNormal3f(0, 0, 1);
		for (i = 0; i < numSections; i++) {
			for (j = 0; j < 8; j++) {
				glVertex3f(sections[i].vertices[sections[i].faces[j].vertices[0]][0], sections[i].vertices[sections[i].faces[j].vertices[0]][1], sections[i].vertices[sections[i].faces[j].vertices[0]][2]);
				glVertex3f(sections[i].vertices[sections[i].faces[j].vertices[1]][0], sections[i].vertices[sections[i].faces[j].vertices[1]][1], sections[i].vertices[sections[i].faces[j].vertices[1]][2]);
				glVertex3f(sections[i].vertices[sections[i].faces[j].vertices[2]][0], sections[i].vertices[sections[i].faces[j].vertices[2]][1], sections[i].vertices[sections[i].faces[j].vertices[2]][2]);
			}
		}
        glEnd();
    };

    void drawNormal() {
        glBegin(GL_LINES);
        for (int i = 0; i < vertices.size(); i++) {
            Vector v(-PI * cos(2 * PI * vertices[i][0] + PI - timeSince / 1000.0), 1, 0);
            v.normalize();
            v = v * 0.1;
            glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
            glVertex3f(vertices[i][0] + v[0], vertices[i][1] + v[1], vertices[i][2] + v[2]);
        }
        glEnd();
    };
};

#endif
