#define blend(a,b,x) ((a) * (1 - (x)) + (b) * (x))
#define whichtile(x,freq) (floor((x) * (freq)))
#define repeat(x,freq)    (mod((x) * (freq), 1.0))



#include "noises.h"

surface robin_mesh_skin(float  Kd =.2;
		float Ka = .1;
	  float Ks = .3;
	  float Kr=.45, rim_width = .3;
	  float iridescence=1;
	  float roughness = .001;
		color specularcolor = .7;
		float transparencyGain=.35;

		float reflmapGain=1.;
		float reflmapGamma=1.;
		float rainbowGain=1.;
		float rainbowGamma=1.;
		float colorNoiseFrequency=1;
		string reflectionenv="";
		color couleurChampi = color(0.09, 0.09,0.09); 
	)
{
	float baseColorFreq = 2;
	float label = 0.5;

	normal	Nf, n;
	n = normalize(N);
	Nf = faceforward(n, I);
	vector i = normalize(-I);
	vector V  = - normalize( I );
	point PP = transform ("shader", P);
	//PP*=colorNoiseFrequency*vfBm(PP/.3*PI, 4.5, 2, 0.05, 0.05);
	color	rainbowColor=((color(noise(2*PI*PP) - .5)) + .05);	

	// reflect
	vector Rcurrent= reflect(I,Nf);
	vector Rworld=vtransform("world", Rcurrent);
	color Cr=color environment(reflectionenv, Rworld);
	Cr = color( reflmapGain*pow(Cr[0], 1/reflmapGamma ), reflmapGain*pow(Cr[1], 1/reflmapGamma ), reflmapGain*pow(Cr[2], 1/reflmapGamma ));

	float  dot = 1 - i.Nf;

	float iridescence1=iridescence*pow( blend(rim_width*1.1, .9, smoothstep(0,1, dot)),  1/rainbowGamma);
//
	
	Oi = 2*blend(rim_width, 1, smoothstep(0,1, dot));
	
	//****************************
	
	/* Transform P from "current" to "shader" */
	point Pshad = transform("shader", P) * baseColorFreq + label;
	/*calculate a very simple noise to drive the spline function */
	float smallnoise = noise(0.7 * Pshad);
	varying color baseColor = couleurChampi;
	/* create variations of the baseColor to pass it to the spline function*/
	//baseColor =  baseColor + color(0.16,0.21,0.21);
	
	color dargre = baseColor - 0.045;
	color midargre = baseColor - 0.025;
	color midgre = baseColor;
	color midligre = baseColor - 0.0575;
	color lightgre = color (0.075,0.075,0.05);
	lightgre /= 2;
	/* use the spline function to color the base of the watermelon */
	color sc = spline(smallnoise,dargre
	,midargre
	,midgre
	,midligre
	,midligre
	,midgre
	,midargre
	,midgre
	,midligre
	,lightgre);

	
	//*****************
	//Ci = sc* rainbowGain*iridescence1 * (Ka * ambient() + Kd * diffuse (Nf) + specularcolor * Ks * specular (Nf, i, roughness)) +  Kr * Cr;	
	sc*=1.3;
	Ci = ( sc * (ambient() + 1 * diffuse(Nf)) + 1 * 0.5 * specular(Nf, V, 0.1) );

	Ci *= Oi;	
}
