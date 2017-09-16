#ifndef _INPUT_H_
#define _INPUT_H_

class SynInput
{
public:
	SynInput();
	SynInput(const SynInput&);
	~SynInput();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};

#endif