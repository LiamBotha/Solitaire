#pragma once


#include <SFML/Graphics.hpp>
#include <iostream>
#include <set>
#include <string>

class Solitaire
{
private:
	static const int SCREENWIDTH = 1600;
	static const int SCREENHEIGHT = 900;
	static const int DECKSIZE = 52;
	static const int CARDSIZE = 13;
	static const int SUITESIZE = 4;
	static const int CARDWIDTH = 120;
	static const int CARDHEIGHT = 175; // TODO - Use Screen width to make the cards scale

	const sf::Color CARDBACKCOLOR = sf::Color(64, 55, 171); // Add the rest of the item colors

	struct Card;

	struct Location
	{
		float x = 0;
		float y = 0;
		std::vector<Card*>* lastLocation;
	};

	struct Card
	{
		sf::RectangleShape cardBack;
		bool bIsFaceDown = false;
		int number = -1;
		int suite = -1;
		float x = 0;
		float y = 0;

		Location currentLocation;

		// compare for order.     
		bool operator <(const Card& pt) const
		{
			return (suite == pt.suite && number < pt.number) || (suite < pt.suite);
		}

		bool operator ==(const Card& pt) const
		{
			return (suite == pt.suite && number == pt.number);
		}
	};

	struct GameState
	{
		Solitaire::Card deck[SUITESIZE][CARDSIZE];
		Solitaire::Card* drawnCards[3] = {};
		std::vector<Solitaire::Card*> slot[4] = {};
		std::vector<Solitaire::Card*> cardPiles[7] = {};
		std::vector<Solitaire::Card*> drawingDeck = {};
	};

	Card deck[SUITESIZE][CARDSIZE]; // Deck holding every possible card for reference
	Card* drawnCards[3] = { NULL };
	Card* lastClickedCard = NULL;

	std::vector<GameState> history = {}; // TODO - This isnt great redo to be more performant -> vector<Card*, lastLocation> history?
	std::vector<Card*> heldCardStack = { nullptr };
	std::vector<Card*> slot[SUITESIZE] = {}; // this is the Ace piles
	std::vector<Card*> cardPiles[7] = {}; // this is the stacks of cards your start off with
	std::vector<Card*> drawingDeck = {}; // deck of cards you pull from;

	sf::Clock mouseClock;

	sf::RectangleShape slotBackgrounds[SUITESIZE]; // The rectangles that draw the background for the 4 ace piles
	sf::RectangleShape pileBackgrounds[7];// The rectangles that draw the background for the 7 card piles

	sf::Font font;
	sf::Text undoText;
	sf::Text restartText;

	bool bIsGameOver = false;

public:
	Solitaire()
	{
		GameStart();
	}

private:
	void GameStart();
	void Initialize(sf::RectangleShape  cardStacks[7], sf::RectangleShape  cardSlot[4]);
	void DrawAcePiles(int i, sf::RenderWindow& window, sf::RectangleShape  slotBackgrounds[4]);
	void DrawCardPiles(int i, sf::RenderWindow& window, sf::RectangleShape  pileBackgrounds[7]);
	void DrawDrawnCards(int i, sf::RenderWindow& window);
	void DrawAllCards(sf::RenderWindow& window, sf::RectangleShape  slotBackgrounds[4], sf::RectangleShape  pileBackgrounds[7]);
	void HandlePollEvents(sf::RenderWindow& window, sf::Vector2f& translatedPos, sf::RectangleShape  cardSlot[4], sf::RectangleShape cardStacks[7]);
	void LoadFromHistory();
	void MouseClicked(sf::RenderWindow& window, sf::Vector2f translatedPos, sf::RectangleShape cardStacks[7]);
	void RestartGame();
	void MouseReleased(sf::RectangleShape  cardSlot[4], sf::Vector2f& translatedPos, sf::RectangleShape cardStacks[7]);
	void ResetCardPosition();
	void MoveToSlot(sf::RectangleShape  cardSlot[4], int i);
	void CheckForVictory();
	bool CheckIfDoubleClicked(Card* card, sf::Time timeSinceLastClick);
	void AddToHistory();
	void RemoveCardFromPreviousLocation(Card* cardToRemove);
	void HandleCardGraphics(Card* card, sf::RenderWindow& window);
	void DrawUIText(sf::RenderWindow& window, const std::string textString, sf::Vector2f textPos, int textSize, int drawDirection);
};

