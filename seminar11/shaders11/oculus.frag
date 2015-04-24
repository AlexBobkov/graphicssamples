/*
 * oculusdevice.cpp
 *
 *  Created on: Jul 03, 2013
 *      Author: Bjorn Blissing
 */

#version 330

uniform sampler2D Texture;

out vec4 fragColor;

in vec4 oColor;
in vec2 oTexCoord0;
in vec2 oTexCoord1;
in vec2 oTexCoord2;

void main()
{
   fragColor.r = oColor.r * texture(Texture, oTexCoord0).r;
   fragColor.g = oColor.g * texture(Texture, oTexCoord1).g;
   fragColor.b = oColor.b * texture(Texture, oTexCoord2).b;
   fragColor.a = 1.0;
   
   //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}