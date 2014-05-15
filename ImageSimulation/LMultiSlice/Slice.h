#pragma once

class Slice {
private:
	Slice(void);
public:
	Slice(size_t id, float lowerBound, float upperBound);
	~Slice(void);

	AModel::Cortege operator[](size_t i);
	
	size_t	getId();
	float	getUpperBound();
	float	getTheUppestBound();
	float	getLowerBound();
	float	getTheLowestBound();

	std::vector<AModel::Cortege*>::iterator begin();
	std::vector<AModel::Cortege*>::iterator end();

	bool	empty();
	void	push_back(AModel::Cortege *atom);
	void	pop_back();
	void	clear();
	size_t	size();
	
	bool	existsByZ(double Z) { return Z < this->upperBound && Z > this->lowerBound; }

private:
	std::vector<AModel::Cortege*>	composition;
	
	size_t	id;
	float	upperBound;
	float	theUppestBound;
	float	lowerBound;
	float	theLowestBound;
};