/* To Render FumiGen meshes*/
void vor_f1_3d( point P;
		float jitter;
		output float f1;
		output point pos1;
)
{
	point thiscell = point(floor(xcomp(P))+0.5, floor(ycomp(P))+0.5, floor(zcomp(P)+0.5));
	f1 = 100000;
	uniform float i,j,z;
	for(i=-1; i<=1; i+=1)
	{
		for(j=-1; j<=1; j+=1)
		{
			for(z=-1; z<=1; z+=1)
			{
				point testCell = thiscell + vector(i,j,z);
				point pos = testCell + jitter*(vector cellnoise(testCell) - 0.5);
				float dist = distance(pos, P);
				if(dist<f1)
				{
					f1 = dist;
					pos1 = pos;
				}
			}
		}
	}
	// Normalized distance to P to its closest point
	f1 = sqrt(f1); 
	f1 /= (1.732);
}

float biasFunc(float t; float a;)
{
	return pow(t, -(log(a)/log(2)));
}

float gainFunc(float t; float a;)
{
	float g;
	if(t<=0.5)
	{
		g = 0.5*biasFunc(2*t, a);
	}
	else
	{
		g = 1.0 - 0.5*biasFunc(2.0-2*t, a);
	}
	return g;
}

displacement robin_mesh(	
			float freq=5.0;
			float gamma=1.0;
			float jitter=1;
			float mixf=1.2;
			float disp=0.15;
)
{
	float d;
	point Pf;

	point PP = transform("shader", P);
	vor_f1_3d(PP*freq, jitter, d, Pf);

	d = pow(d, gamma);
	d = mix(d, 1-d, mixf);

	vector toAdd = disp*d * ntransform("current", normalize(Pf-P));
	P += toAdd;
	N = calculatenormal(P);
}

