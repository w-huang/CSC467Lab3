{
	/*** ASSIGNMENT DECLARATION ***/
	const int ERROR = 1000;
	/* These init vals mean error unless lower levels
	 * initialize new ones properly.
	 */
	/* const float fcol_init_val = -1000.0;
	 * const int icol_init_val = -1000;
	 * THESE INITs DON'T WORK BECAUSE OF THE 
	 * '!' OPERATOR, WHICH TECHNICALLY MAKES THEM
	 * NOT CONSTANTS.
	 */ 
	
	const float fcol_init_val = 1000.0;
	const int icol_init_val = 1000;
	ivec4 retcol_int;
	vec4 retcol_float;
	bvec4 retcol_bool;
	int retval = -1;
	int colour_index = 0;
	const int level = 0;
	const vec4 amb_col = gl_Light_Ambient;
	const vec4 mat_shine = gl_Material_Shininess;
	{ 
		/* use local colour from global initialization */
		int level = 1;
		float fcol_init_val = 1.0;
		int icol_init_val = 1;
		vec4 current_col = vec4(fcol_init_val,fcol_init_val,fcol_init_val,fcol_init_val);
		ivec4 ideal_col = ivec4(icol_init_val,icol_init_val,icol_init_val,icol_init_val);
		bvec4 success_vec = bvec4(false,false,false,false);
		int colour_index = (colour_index+1)*level;
		
		/************************************************/
		/* make current_col the same as the ambient col */
		/************************************************/
		if(current_col[0] == -1000.0 ||
		   current_col[1] == -1000.0 || 
		   current_col[2] == -1000.0 ||
		   current_col[3] == -1000.0) {
			retval = ERROR;
		}
		

		if(current_col[0] != amb_col[0]){
			current_col[0] = amb_col[0];
		}
		else{
			current_col[0] = amb_col[0]+1.0;
		}
		success_vec[0] = true;

		if(current_col[1] != amb_col[1]){
			current_col[1] = amb_col[1];
		}
		else{
			current_col[1] = amb_col[1]+1.0;
		}
		success_vec[1] = true;

		if(current_col[2] != amb_col[2]){
			current_col[2] = amb_col[2];
		}
		else{
			current_col[2] = amb_col[2]+1.0;
		}
		success_vec[2] = true;
		
		if(current_col[3] != amb_col[3]){
			current_col[3] = amb_col[3];
		}
		else{
			current_col[3] = amb_col[3]+1.0;
		}
		success_vec[3] = true;

		if (retval == ERROR){
			/* Only send back the ideal colour if you fail to match the ambient colour. */
			retcol_int = ideal_col;
			retcol_float = vec4(-fcol_init_val,-fcol_init_val,-fcol_init_val,-fcol_init_val);
			retcol_bool = bvec4(false,false,false,false);
		}
	}
	{
/*
vec4 gl_FragColor (result)
bool gl_FragDepth (result) 
vec4 gl_FragCoord (result) 
vec4 gl_TexCoord (attribute) 
vec4 gl_Color (attribute) 
vec4 gl_Secondary (attribute)
vec4 gl_FogFragCoord (attribute) 
vec4 gl_Light_Half(uniform) 
vec4 gl_Light_Ambient(uniform)
vec4 gl_Material_Shininess(uniform) 
vec4 env1(uniform)
vec4 env2(uniform) 
vec4 env3 (uniform)
*/
		const float level = 1.0;
		vec4 multiplier = vec4(0.0,0.0,0.0,0.0);	
		vec4 thisLevelShade = vec4(1.0,1.0,0.0,0.0);
		vec4 thisLevelTexture; 
		/*** Call Functions for Calculations done on this Level (initialized above) ***/
		float result = dp3(mat_shine,env1);
		thisLevelTexture[0] = gl_TexCoord[0] * gl_Light_Half[1] * result;
		thisLevelTexture = gl_TexCoord * thisLevelTexture[2] * 2.4;
 		multiplier[0] = dp3(thisLevelTexture,mat_shine);
 		multiplier[1] = dp3(multiplier,multiplier); 
 		multiplier[3] = gl_Material_Shininess[0];
 		multiplier = lit(multiplier);
 		thisLevelShade[0] = (thisLevelShade[0] + multiplier[0]) * level;
 		thisLevelShade = (thisLevelShade + multiplier);
 		thisLevelShade[2] = thisLevelShade[2] + multiplier[3];
 		gl_FragColor = thisLevelShade;
	}

}
