// *********************************************************
// Model Class
// Author: Axel Schumacher (axelschumacherberry@gmail.com).
//
// Description:
// Provide current model state information, such as AA settings and others...
// *********************************************************

#pragma once

enum AntiAliasingType
{
	NO_ANTIALIASING,
	UNIFORM,
	PENTAGONAL,
	STOCHASTIC
};

class Model
{
	public:
		/** Return the model instance of the program */
		static Model *getInstance();

		/** Set the antiAliasingType */
		void setAntiAliasingType(AntiAliasingType type);

		/** Get the antiAliasingType */
		AntiAliasingType getAntiAliasingType() const;

		/** Set the antiAliasingRaysPerPixel */
		void setAntiAliasingRaysPerPixel(unsigned int raysPerPixel);

		/** Get the antiAliasingRaysPerPixel */
		unsigned int getAntiAliasingRaysPerPixel() const;

	private:
		/** AntiAliasing to be used*/
		AntiAliasingType antiAliasingType(NO_ANTIALIASING);

		/** Number of rays used for each pixel */
		unsigned int antiAliasingRaysPerPixel(1);
};
