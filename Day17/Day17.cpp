#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <bitset>
#include <deque>

bool CanPushLeft(const std::vector<std::bitset<7>>& piece, const std::vector<std::bitset<7>>& tower, std::size_t row)
{
	for (std::size_t r{ 0 }; r < piece.size(); r++)
	{
		auto testOR = (piece[r] << 1) | tower[row + r];
		if (testOR.count() != piece[r].count() + tower[row + r].count())
		{
			return false;
		}
	}

	return true;
}

bool CanPushRight(const std::vector<std::bitset<7>>& piece, const std::vector<std::bitset<7>>& tower, std::size_t row)
{
	for (std::size_t r{ 0 }; r < piece.size(); r++)
	{
		auto testOR = (piece[r] >> 1) | tower[row + r];
		if (testOR.count() != piece[r].count() + tower[row + r].count())
		{
			return false;
		}
	}

	return true;
}

void PushLeft(std::vector<std::bitset<7>>& piece)
{
	for (auto& row : piece)
	{
		row <<= 1;
	}
}

void PushRight(std::vector<std::bitset<7>>& piece)
{
	for (auto& row : piece)
	{
		row >>= 1;
	}
}

bool CanDrop(const std::vector<std::bitset<7>>& piece, const std::vector<std::bitset<7>>& tower, std::size_t row)
{
	if (row == 0)
	{
		return false;
	}

	for (std::size_t r{ 0 }; r < piece.size(); r++)
	{
		auto testOR = piece[r] | tower[row + r - 1];
		if (testOR.count() != piece[r].count() + tower[row + r - 1].count())
		{
			return false;
		}
	}

	return true;
}

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::string jets;
	std::getline(inStrm, jets);

	const std::vector<std::vector<std::bitset<7>>> pieces{
		{0b0011110},
		{0b0001000, 0b0011100, 0b0001000},
		{0b0011100, 0b0000100, 0b0000100},
		{0b0010000, 0b0010000, 0b0010000, 0b0010000},
		{0b0011000, 0b0011000}
	};

	std::vector<std::bitset<7>> tower(7);
	std::size_t bottomEmptyRowIndex{ 0 };

	std::size_t jetId{ 0 };
	for (std::size_t pieceId{ 0 }; pieceId < 2022; pieceId++)
	{
		std::vector<std::bitset<7>> fallingPiece = pieces[pieceId % pieces.size()];
		bool landed = false;

		std::size_t pieceRowIndex = bottomEmptyRowIndex + 3;
		if (pieceRowIndex + fallingPiece.size() >= tower.size())
		{
			tower.resize(pieceRowIndex + 20);	//add some extra margin for efficiency
		}

		while (!landed)
		{
			//Piece is pushed by a jet of hot gas
			if (jets[jetId++ % jets.size()] == '>')
			{
				if (CanPushRight(fallingPiece, tower, pieceRowIndex))
				{
					PushRight(fallingPiece);
				}
			}
			else
			{
				if (CanPushLeft(fallingPiece, tower, pieceRowIndex))
				{
					PushLeft(fallingPiece);
				}
			}

			//Piece drops
			if (pieceRowIndex > bottomEmptyRowIndex || CanDrop(fallingPiece, tower, pieceRowIndex))
			{
				pieceRowIndex--;
			}
			else
			{
				for (std::size_t r{ 0 }; r < fallingPiece.size(); r++)
				{
					tower[pieceRowIndex + r] |= fallingPiece[r];
				}
				landed = true;
			}
		}

		bottomEmptyRowIndex = std::max(bottomEmptyRowIndex, pieceRowIndex + fallingPiece.size());
	}

	std::cout << bottomEmptyRowIndex << "\n";
}
