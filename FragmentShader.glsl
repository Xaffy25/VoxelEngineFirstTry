#version 460

out vec4 fragColor;

in vec3 vPos;

uniform sampler3D uVolumeTexture;
uniform vec3 uColorTable[8];
uniform float uGrayPalette[16];
uniform vec3 uBoxMin;
uniform vec3 uBoxMax;
uniform vec3 uCameraPos;
uniform mat4 uModel;
uniform vec3 sunDir;

const float stepSize = 0.002;
const int maxSteps = 1000;

vec3 TextureSize = textureSize(uVolumeTexture,0);

vec4 GetColorFromPalette(int id)
{
	return vec4(vec3(uColorTable[clamp(id,0,7)]),1.0);
}

vec4 getGrayFromPalette(int id) 
{
	return vec4(uGrayPalette[clamp(id, 0, 15)], uGrayPalette[clamp(id, 0, 15)], uGrayPalette[clamp(id, 0, 15)], 1.0);
}

vec3 RayDirection(vec3 origin, vec3 hitPos)
{
	return normalize(hitPos - origin);
}

bool AABBIntersect(vec3 rayOrigin, vec3 rayDir, out vec3 hitPos)
{
	
	if (rayOrigin.x > uBoxMin.x && rayOrigin.y > uBoxMin.y && rayOrigin.z > uBoxMin.z && rayOrigin.x < uBoxMax.x && rayOrigin.y < uBoxMax.y && rayOrigin.z < uBoxMax.z)
	{
		hitPos = rayOrigin;
		return true;
	}
	vec3 invDir = 1.0/rayDir;

	vec3 tMin = (uBoxMin - rayOrigin) * invDir;
	vec3 tMax = (uBoxMax - rayOrigin) * invDir;

	vec3 t1 = min(tMin,tMax);
	vec3 t2 = max(tMin,tMax);

	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x,t2.y),t2.z);

	if(tNear < tFar && tFar > 0.0)
	{
		hitPos = rayOrigin + tNear * rayDir;
				
		vec3 pos = hitPos;
		return true;
	}

	return false;
}

int GetVoxelData(vec3 XYZ)
{
	vec3 texCoords = XYZ/TextureSize;
	
	float val = texture(uVolumeTexture,texCoords).r * 255;

	return int(val);
}

vec3 boxDimensions = uBoxMax - uBoxMin;
vec3 voxelSize = boxDimensions / TextureSize;
vec3 normal = vec3(0.0);

vec4 TraverseVoxelGrid (vec3 start, vec3 dir)
{
	start += 0.00001*dir;
	vec3 tDelta = abs(voxelSize / dir);

	vec3 startCoords = (start - uBoxMin) / (uBoxMax - uBoxMin);

	startCoords = floor(startCoords * TextureSize) / TextureSize;
		
	startCoords = clamp(startCoords, 0.0, 1.0); 

	ivec3 currentVoxel = ivec3(floor((start - uBoxMin) / voxelSize));

	int ID = GetVoxelData(currentVoxel);
	vec3 tMax;
	tMax.x = ((dir.x > 0 ? (currentVoxel.x + 1) : currentVoxel.x) * voxelSize.x + uBoxMin.x - start.x) / dir.x;
	tMax.y = ((dir.y > 0 ? (currentVoxel.y + 1) : currentVoxel.y) * voxelSize.y + uBoxMin.y - start.y) / dir.y;
	tMax.z = ((dir.z > 0 ? (currentVoxel.z + 1) : currentVoxel.z) * voxelSize.z + uBoxMin.z - start.z) / dir.z;

	int StepX = (dir.x > 0) ? 1 : (dir.x < 0) ? -1 : 0;
	int StepY = (dir.y > 0) ? 1 : (dir.y < 0) ? -1 : 0;
	int StepZ = (dir.z > 0) ? 1 : (dir.z < 0) ? -1 : 0;


	float current_X_index = max(1,ceil(start.x - uBoxMin.x));
	float current_Y_index = max(1,ceil(start.y - uBoxMin.y));
	float current_Z_index = max(1,ceil(start.z - uBoxMin.z));

	float tDeltaX = 1/dir.x;
	float tDeltaY = 1/dir.y;
	float tDeltaZ = 1/dir.z;

	if (ID > 0)
	{
		//Need to somehow get normals for chunk sides

		vec4 voxelcolor = GetColorFromPalette(ID);//getGrayFromPalette(ID);
		vec3 _vPos = (uModel * vec4(uBoxMin + (vec3(currentVoxel) + 0.5) * voxelSize, 1.0)).xyz;

		gl_FragDepth = length(_vPos - uCameraPos) / 100.0;

		return voxelcolor;
		float lightAtt;

		lightAtt = max(dot(normal,normalize(-sunDir)),0.0);

		vec4 col = (voxelcolor * 0.1 + voxelcolor * lightAtt);
		col.w = 1.0;
		return col;
			
	}

	int steps = 0;

	while(steps < maxSteps)
	{
		normal = vec3(0.0);
		
		 if (tMax.x < tMax.y) {
			if (tMax.x < tMax.z) {
				// Step in x-direction
				currentVoxel.x += int(sign(dir.x));
				tMax.x += tDelta.x;
				normal.x = -StepX;
			}
			else {
				// Step in z-direction
				currentVoxel.z += int(sign(dir.z));
				tMax.z += tDelta.z;
				normal.z = -StepZ;
			}
		} else {
			if (tMax.y < tMax.z) {
				// Step in y-direction
				currentVoxel.y += int(sign(dir.y));
				tMax.y += tDelta.y;
				normal.y = -StepY;
			 } else {
				// Step in z-direction
				currentVoxel.z += int(sign(dir.z));
				tMax.z += tDelta.z;
				normal.z = -StepZ;
			}
		}
		//something is wrong as it just cuts some fragments in non voxel way
		if(currentVoxel.x+1 > TextureSize.x || currentVoxel.x < 0 || currentVoxel.y+1 > TextureSize.y || currentVoxel.y < 0 ||currentVoxel.z+1 > TextureSize.z || currentVoxel.z < 0) break;

		//Check next voxel
		ID = GetVoxelData(currentVoxel);

	
		if (ID > 0)
		{
			vec4 voxelcolor = GetColorFromPalette(ID);//getGrayFromPalette(ID);
			//ensure precision errors dont occur
			if(voxelcolor.x == -1.0)
			{
				continue;
			}
			vec3 _vPos = (uModel * vec4(uBoxMin + (vec3(currentVoxel) + 0.5) * voxelSize, 1.0)).xyz;
			gl_FragDepth = length(_vPos - uCameraPos) / 100.0;
			return voxelcolor;
			//Basic lighting, need to make sure that no voxels are in way
			float lightAtt;

			lightAtt = max(dot(normal,normalize(-sunDir)),0.0);

			vec4 col = (voxelcolor * 0.1 + voxelcolor * lightAtt);
			col.w = 1.0;


			return col;
		}
		steps++;
	}

	return vec4(0.0);
}



void main() 
{
	vec3 rayDir = normalize((inverse(uModel) * vec4(normalize(vPos - uCameraPos), 0.0)).xyz);
	vec3 rayOrigin = (inverse(uModel) * vec4(uCameraPos, 1.0)).xyz;
	vec3 hitPos = vec3(0.0);

	if(AABBIntersect(rayOrigin,rayDir,hitPos))
	{
		fragColor = TraverseVoxelGrid(hitPos,rayDir);
		//fragColor = RayMarch(hitPos,rayDir);
		if(fragColor.a < 1.0)
			discard;
		
	}
	else
	{
		discard;
	}
}