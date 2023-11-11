#ifndef VOXELS_BLOCK_H_
#define VOXELS_BLOCK_H_

#define BLOCK_MODEL_CUBE 1
#define BLOCK_MODEL_X_SPRITE 2

#define FACE_MX 0
#define FACE_PX 1
#define FACE_MY 2
#define FACE_PY 3
#define FACE_MZ 4
#define FACE_PZ 5

class Block {
public:
	static Block* blocks[256];

	const unsigned int id;
						 //  0 1   2 3   4 5
	int textureFaces[6]; // -x,x, -y,y, -z,z
	unsigned char emission[3];
	unsigned char drawGroup = 0;
	unsigned char model = 1; // 0:None 1:Block 2:XSprite
	bool lightPassing = false;
	bool skyLightPassing = false;
	bool obstacle = true;
	bool selectable = true;
	bool breakable = true;
	bool rotatable = false;
	float hitboxScale = 1;

	Block(unsigned int id, int texture);
};

#endif /* VOXELS_BLOCK_H_ */
