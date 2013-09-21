/* To Render FumiGen meshes*/

surface robin_mesh_color( color baseColor= color(0.01, 0.01, 0.01); )
{
	float Ks = 1.0;
	float roughness = 0.02;
	normal Nf;
	vector T_dummy, R;
	vector IN = normalize(I);
	float fresnelKr,fresnelKt;
	float eta = 1.2;
    	Nf = faceforward (normalize(N), I);

    	fresnel (IN, Nf, 1/eta, fresnelKr, fresnelKt, R, T_dummy);
	Nf = faceforward(normalize(N), I);
	normal Nbump = faceforward(normalize(N), I);
	normal Nspec = normalize( mix (faceforward(normalize(Ng), I), Nbump, clamp(1,0.0,0.5)));
	vector V = normalize(-I);

	Oi = Os;
	//Ci = color(1,1,1) * ( 1 * ambient() + diffuse(normalize(N))) ; // + Ks * specular(Nbump, V, roughness) );
	Ci += color(1,1,1) * 0.5 * fresnelKr;

	Ci *= Oi;
}
