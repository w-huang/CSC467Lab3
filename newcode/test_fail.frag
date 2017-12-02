{
	const int ERROR = -1000;
	const float fcol_init_val = -1000.0;
	const int icol_init_val = -1000;
	
	/* Redefinition */
	const float fcol_init_val = 1000.0;

	/* Redefinition without qualifier */
	int icol_init_val;
	/* Redefinition as different type */
	vec4 icol_init_val;

	ivec4 retcol_int;
	vec4 retcol_float;
	bvec4 retcol_bool;
	int retval = -1;
	int colour_index = 0;
	const int level = 0;
	const vec4 amb_col = gl_Light_Ambient;
	const vec4 mat_shine = gl_Material_Shininess;

	/* Variable keeps first definition when reporting errors
	 * We will assume that this is the right thing to do.
	 */
	icol_init_val=1000;
	
	{ 
		/* use local colour from global initialization */
		int level = 1;
		float fcol_init_val = 1.0;
		int icol_init_val = 1;
		
		/*Initialization with vec3 and type of var is vec3 but 4 init vals supplied*/
		/*Using an int for the vec3 initialization*/
		/*using a non-constant for initialization -- This is fine*/
		/*Logical error inside of one of the initial values*/
		/*Using another vector for initialization*/
		ivec3 current_col = vec3(level,1.0+3.0,1 && 3.0,gl_Light_Half);
		
		/* ivec3 mismatch with ivec4 */
		ivec3 ideal_col = ivec4(icol_init_val,icol_init_val,icol_init_val,icol_init_val);
		
		bvec4 success_vec = bvec4(false,false,false,false);
		
		/* Indexing a non-vector type */
		/* Multiplying with Boolean*/
		int colour_index = (colour_index[0]+success_vec)*true;
		colour_index = true * true;	
		/************************************************/
		/* make current_col the same as the ambient col */
		/************************************************/
		/*Index out of bounds for current_col*/
		/*Comparison with wrong type*/
		/*Assignment to Const ERROR */
		if(current_col[3] == 1000.0*-2.1 ||
		   current_col[0] == 1000 || 
		   current_col[1] == 1000 ||
		   current_col[2] == 1000) {
			ERROR = retval;
		}

		/*Doesn't report the right error*/
		/* Does error out though */
		amb_col = amb_col + 3;	
	
		/*Assignment to wrong vector type and size*/
		if(1.0 != amb_col[0]){
			current_col = gl_Light_Ambient;
		}
		else{
		/*Arithmetic with wrong type from uniform variable*/
			current_col[0] = amb_col+1;
		}
		success_vec[0] = true;

		if(current_col[1] != amb_col[1]){
			current_col[1] = amb_col[1];
		}
		else{
			current_col[1] = amb_col+1.0;
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

		/* Incorrect if statement since no boolean result*/
		if (retval + ERROR){
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
		const float level = gl_FragColor;
		const vec4 report_err = gl_FragColor;

		vec4 multiplier = vec4(0.0,0.0,0.0,0.0);	
		vec4 thisLevelShade = vec4(1.0,1.0,0.0,0.0);
		vec4 thisLevelTexture; 
		/*** Call Functions for Calculations done on this Level (initialized above) ***/
		/* Not enough params for function */
		/* Using variable defined in another parallel scope */
		float result = dp3(current_col);
			
		if (true){
			/* Cannot assign to result variable inside
			 * scope of IF statement as lab said
			 */
			report_err = gl_FragColor;
		}
		thisLevelTexture[0] = gl_TexCoord[0] * gl_Light_Half[1] * result;
		/* Boolean operation with incorrect type */
		thisLevelTexture = gl_TexCoord && thisLevelTexture[2] * 2.4;
 		multiplier[0] = dp3(thisLevelTexture,mat_shine);
 		multiplier[1] = dp3(multiplier,multiplier); 
 		multiplier[3] = gl_Material_Shininess[0];
		/* Input is basic type, not vector */
 		multiplier = lit(multiplier[0]);
		/* No args provided, one needed */
 		multiplier = lit();
		/* Too many arguments */
		/* also variables not declared */
 		multiplier = lit(a,b,c);
		
 		thisLevelShade[0] = (thisLevelShade[0] + multiplier[0]) * level;
 		thisLevelShade = (thisLevelShade + multiplier);
 		thisLevelShade[2] = thisLevelShade[2] + multiplier[3];
 		gl_FragColor = thisLevelShade;
	}

}
