/*
 * Flag.cpp
 *
 *  Created on: 19/11/2014
 *      Author: leonardo
 */
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "FlagShadder.h"

FlagShadder::FlagShadder() {
	text = new CGFtexture("seaText.jpg");
	init("flag.vert", "flag.frag");
	CGFshader::bind();

	startTime = 0.0;
	time = 0.0;
	timeLoc = glGetUniformLocation(id(), "time");
	angle = 0.0;

	angleLoc = glGetUniformLocation(id(), "angle");

	baseTextLoc = glGetUniformLocation(id(), "baseTexture");

	glUniform1i(baseTextLoc, 0);

	CGFshader::unbind();

}
FlagShadder::~FlagShadder() {

}

void FlagShadder::bind() {
	CGFshader::bind();

	// update uniforms
	glUniform1f(angleLoc, angle);
	glUniform1f(timeLoc, time);
	// make sure the correct texture unit is active
	glActiveTexture (GL_TEXTURE0);

	// apply/activate the texture you want, so that it is bound to GL_TEXTURE0
	text->apply();

}
void FlagShadder::unbind() {
	CGFshader::unbind();
}
void FlagShadder::update(unsigned long t) {
	if (startTime == 0) {
		startTime = t;
	} else {
		time = t - startTime;
	}

}
