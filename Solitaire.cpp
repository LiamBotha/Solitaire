#include "Solitaire.h"

void Solitaire::GameStart()
{
	sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Solitaire");

	window.setKeyRepeatEnabled(false); // input only triggers once when pressed
	window.setFramerateLimit(60);

	Initialize(pileBackgrounds, slotBackgrounds);

	sf::Vector2f translatedPos;
	sf::Vector2f lastPos;

	if (font.loadFromFile("Fonts/LiberationSans.ttf"))
	{
		undoText.setFont(font);
		undoText.setString("UNDO");
		undoText.setCharacterSize(24);
		undoText.setFillColor(sf::Color::White);
		undoText.setOutlineColor(sf::Color::Black);
		undoText.setOutlineThickness(1);
		undoText.setStyle(sf::Text::Bold);
		sf::FloatRect boundingBox = undoText.getGlobalBounds();
		undoText.setPosition(sf::Vector2f(SCREENWIDTH - 135, SCREENHEIGHT - 50));

		restartText.setFont(font);
		restartText.setString("RESTART");
		restartText.setCharacterSize(24);
		restartText.setFillColor(sf::Color::White);
		restartText.setOutlineColor(sf::Color::Black);
		restartText.setOutlineThickness(1);
		restartText.setStyle(sf::Text::Bold);
		restartText.setPosition(sf::Vector2f(40, SCREENHEIGHT - 50));
	}

	while (window.isOpen())
	{
		window.clear(sf::Color(15, 102, 30));

		lastPos = translatedPos;
		auto mousePos = sf::Mouse::getPosition(window);
		translatedPos = window.mapPixelToCoords(mousePos);

		HandlePollEvents(window, translatedPos, slotBackgrounds, pileBackgrounds);

		// Sets the held card stack to mouse pos
		if (heldCardStack.size() != 0)
		{
			int parentX = translatedPos.x - (CARDWIDTH / 2);
			int parentY = translatedPos.y - (CARDHEIGHT / 2);

			sf::Vector2f offset = translatedPos - lastPos;

			for (int i = 0; i < heldCardStack.size(); i++)
			{
				if (heldCardStack[i] != NULL)
				{
					//heldCardStack[i]->x = parentX;
					//heldCardStack[i]->y = parentY + (i * 30);

					heldCardStack[i]->x += offset.x;
					heldCardStack[i]->y += offset.y;
				}
			}
		}

		// Updates all the cards to their current position 
		for (int i = 0; i < SUITESIZE; i++)
		{
			for (int j = 0; j < CARDSIZE; j++)
			{
				deck[i][j].cardBack.setPosition(deck[i][j].x, deck[i][j].y);
			}
		}

		// Handles the drawing for each of the stacks, the deck, and held cards
		DrawAllCards(window, slotBackgrounds, pileBackgrounds);

		window.draw(undoText);
		window.draw(restartText);

		CheckForVictory();

		if (bIsGameOver)
		{
			DrawUIText(window, "Victory", sf::Vector2f(SCREENWIDTH / 2, SCREENHEIGHT / 2), 75, 2);
		}

		window.display();
	}
}

void Solitaire::DrawAllCards(sf::RenderWindow& window, sf::RectangleShape  slotBackgrounds[4], sf::RectangleShape  pileBackgrounds[7])
{
	// Draws the card piles, drawn cards and their backgrounds
	for (int i = 0; i < 7; i++)
	{
		DrawAcePiles(i, window, slotBackgrounds);

		DrawCardPiles(i, window, pileBackgrounds);

		DrawDrawnCards(i, window);
	}

	// Draws the deck of remaining cards
	for (int i = 0; i < drawingDeck.size(); i++)
	{
		Card* card = drawingDeck[i];

		if (card->bIsFaceDown)
		{
			card->cardBack.setFillColor(CARDBACKCOLOR);
		}

		window.draw(card->cardBack);

		HandleCardGraphics(card, window);
	}

	//Draws the stack of cards held by the player
	if (heldCardStack.size() != 0)
	{
		for (int i = 0; i < heldCardStack.size(); i++)
		{
			if (heldCardStack[i] != NULL)
			{
				window.draw(heldCardStack[i]->cardBack);

				HandleCardGraphics(heldCardStack[i], window);
			}
		}
	}
}

void Solitaire::DrawDrawnCards(int i, sf::RenderWindow& window)
{
	if (i > 2)
		return;

	Card* card = drawnCards[i];

	if (card == NULL)
		return;
	else if (heldCardStack.size() != 0 && card == heldCardStack[0])
		return;

	if (card->suite == 1 || card->suite == 3 && card->cardBack.getFillColor() != sf::Color::Red)
		card->cardBack.setFillColor(sf::Color::Red);
	else if (card->suite == 0 || card->suite == 2 && card->cardBack.getFillColor() != sf::Color::White)
		card->cardBack.setFillColor(sf::Color::White);

	window.draw(card->cardBack);

	HandleCardGraphics(card, window);
}

void Solitaire::DrawCardPiles(int i, sf::RenderWindow& window, sf::RectangleShape  pileBackgrounds[7])
{
	int length = cardPiles[i].size();

	if (length != 0 && heldCardStack.size() != 0 && cardPiles[i][0] == heldCardStack[0])
	{
		window.draw(pileBackgrounds[i]);
	}
	else if (length == 0)
	{
		window.draw(pileBackgrounds[i]);
		return;
	}

	if (cardPiles[i][length - 1] != NULL)
		cardPiles[i][length - 1]->bIsFaceDown = false;

	for (int j = 0; j < length; j++)
	{
		Card* card = cardPiles[i][j];

		if (card->bIsFaceDown)
			card->cardBack.setFillColor(CARDBACKCOLOR);
		else if (card->suite == 1 || card->suite == 3)
			card->cardBack.setFillColor(sf::Color::Red);
		else
			card->cardBack.setFillColor(sf::Color::White);

		window.draw(card->cardBack);

		HandleCardGraphics(card, window);

	}
}

void Solitaire::DrawAcePiles(int i, sf::RenderWindow& window, sf::RectangleShape  slotBackgrounds[4])
{
	if (i > (SUITESIZE - 1))
		return;

	int length = slot[i].size();

	if (length < 2)
	{
		window.draw(slotBackgrounds[i]);
	}

	if (length > 0)
	{
		if (heldCardStack.size() != 0 && slot[i][length - 1] == heldCardStack[0] && length > 1)
		{
			auto card = slot[i][length - 2];

			if (card->suite == 1 || card->suite == 3)
				card->cardBack.setFillColor(sf::Color::Red);
			else
				card->cardBack.setFillColor(sf::Color::White);

			window.draw(card->cardBack);

			HandleCardGraphics(card, window);
		}

		auto card = slot[i][length - 1];

		if (card->suite == 1 || card->suite == 3)
			card->cardBack.setFillColor(sf::Color::Red);
		else
			card->cardBack.setFillColor(sf::Color::White);

		window.draw(card->cardBack);

		HandleCardGraphics(slot[i][length - 1], window);
	}
}

void Solitaire::Initialize(sf::RectangleShape cardStacks[7], sf::RectangleShape cardSlot[4])
{
	/* initialize random seed: */
	srand(time(NULL));

	for (int i = 0; i < 7; i++)
	{
		cardStacks[i].setOutlineColor(sf::Color::Black);
		cardStacks[i].setOutlineThickness(2);
		cardStacks[i].setFillColor(sf::Color::Green);
		cardStacks[i].setSize(sf::Vector2f(CARDWIDTH, CARDHEIGHT));
		cardStacks[i].setPosition(30.0f + (CARDWIDTH * 1.4) * (i + 1), 30.0f);
	}

	for (int i = 0; i < SUITESIZE; i++)
	{
		cardSlot[i].setOutlineColor(sf::Color::Black);
		cardSlot[i].setOutlineThickness(2);
		cardSlot[i].setFillColor(sf::Color(5, 66, 15));
		cardSlot[i].setSize(sf::Vector2f(CARDWIDTH, CARDHEIGHT));
		cardSlot[i].setPosition(SCREENWIDTH - (CARDWIDTH + 30), 30.0f + ((CARDHEIGHT + 30.0f) * i));

		for (int j = 0; j < CARDSIZE; j++)
		{
			deck[i][j].cardBack.setSize(sf::Vector2f(CARDWIDTH, CARDHEIGHT));
			deck[i][j].cardBack.setOutlineColor(sf::Color::Black);
			deck[i][j].cardBack.setOutlineThickness(2);

			if (i == 1 || i == 3)
				deck[i][j].cardBack.setFillColor(sf::Color::Red);
			else
				deck[i][j].cardBack.setFillColor(sf::Color::White);

			deck[i][j].number = j;
			deck[i][j].suite = i;

			deck[i][j].x = 30;
			deck[i][j].y = 30;
			deck[i][j].bIsFaceDown = true;

			Location newLocation;
			newLocation.lastLocation = &drawingDeck;
			newLocation.x = 30;
			newLocation.y = 30;
			deck[i][j].currentLocation = newLocation;

			drawingDeck.push_back(&deck[i][j]);

			//std::cout << "Card Num: " << deck[i][j].number + 1 << " Suite: " << deck[i][j].suite + 1 << std::endl;
		}
	}

	std::set<Card> cardSet; // switch to int array and random 1 - 52

	for (int i = 0, k = 1; i < 7; i++)
	{
		for (int j = 0; j < k; j++)
		{
			int randCardNum = rand() % drawingDeck.size();
			Card* card = drawingDeck[randCardNum];
			drawingDeck.erase(drawingDeck.begin() + randCardNum);

			Location newLocation;
			newLocation.lastLocation = &cardPiles[i];
			newLocation.x = (30.0f + (CARDWIDTH * 1.4) * (i + 1));;
			newLocation.y = 30.0f * (j + 1);
			card->currentLocation = newLocation;

			cardSet.insert(*card);
			cardPiles[i].push_back(card);

			cardPiles[i][j]->x = (30.0f + (CARDWIDTH * 1.4) * (i + 1));
			cardPiles[i][j]->y = 30.0f * (j + 1);

			if (j >= (k - 1))
			{
				cardPiles[i][j]->bIsFaceDown = false;
			}
		}
		k++;
	}
}

void Solitaire::HandlePollEvents(sf::RenderWindow& window, sf::Vector2f& translatedPos, sf::RectangleShape cardSlot[4], sf::RectangleShape cardStacks[7])
{
	sf::Event event;
	if (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		if (bIsGameOver == false)
		{
			if (event.type == sf::Event::MouseButtonPressed)
			{
				MouseClicked(window, translatedPos, cardStacks);
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				MouseReleased(cardSlot, translatedPos, cardStacks);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
			{
				LoadFromHistory();
			}
		}
	}
}

void Solitaire::MouseReleased(sf::RectangleShape cardSlot[4], sf::Vector2f& translatedPos, sf::RectangleShape cardStacks[7])
{
	if (heldCardStack.size() > 0)
	{
		// Checks if card dropped in ace stack
		for (int i = 0; i < SUITESIZE; i++)
		{
			if (cardSlot[i].getGlobalBounds().intersects(heldCardStack[0]->cardBack.getGlobalBounds()))
			{
				if (heldCardStack.size() != 1)
				{
					ResetCardPosition();
					heldCardStack.clear();
					return;
				}
				else if (slot[i].size() == 0 && heldCardStack[0]->number != 0)
				{
					ResetCardPosition();
					heldCardStack.clear();
					return;
				}
				else if (slot[i].size() > 0 && (heldCardStack[0]->number != (slot[i].back()->number + 1) || heldCardStack[0]->suite != slot[i].back()->suite))
				{
					ResetCardPosition();
					heldCardStack.clear();
					return;
				}

				MoveToSlot(cardSlot, i);

				return;
			} // hovering over one of the 4 stacks
		}

		// Checks if selected card dropped in a card pile/stack
		for (int i = 0; i < 7; i++)
		{
			int length = cardPiles[i].size();
			if (length == 0)
			{
				if (cardStacks[i].getGlobalBounds().intersects(heldCardStack[0]->cardBack.getGlobalBounds()) && heldCardStack[0]->number == 12)
				{
					AddToHistory();

					for (int j = 0; j < heldCardStack.size(); j++)
					{
						if (heldCardStack[j]->suite == 1 || heldCardStack[j]->suite == 3)
							heldCardStack[j]->cardBack.setFillColor(sf::Color::Red);
						else
							heldCardStack[j]->cardBack.setFillColor(sf::Color::White);

						heldCardStack[j]->x = (30.0f + (CARDWIDTH * 1.4) * (i + 1));
						heldCardStack[j]->y = (30.0f * (length + 1) + (j * 30));

						Location oldLocation = heldCardStack[j]->currentLocation;
						Location newLocation;
						newLocation.lastLocation = &cardPiles[i];
						newLocation.x = (30.0f + (CARDWIDTH * 1.4) * (i + 1));;
						newLocation.y = (30.0f * (length + 1) + (j * 30));
						heldCardStack[j]->currentLocation = newLocation;

						cardPiles[i].push_back(heldCardStack[j]);

						RemoveCardFromPreviousLocation(heldCardStack[j]);
					}

					heldCardStack.clear();
					return;
				}
			}
			else
			{
				Card* card = cardPiles[i][length - 1];

				if (card->cardBack.getGlobalBounds().intersects(heldCardStack[0]->cardBack.getGlobalBounds())) // hovering over one of the 7 piles
				{
					int pileSuite = card->suite % 2;
					int heldSuite = heldCardStack[0]->suite % 2;

					if (card->number == (heldCardStack[0]->number + 1) && pileSuite != heldSuite)
					{
						AddToHistory();

						for (int j = 0; j < heldCardStack.size(); j++)
						{
							if (heldCardStack[j]->suite == 1 || heldCardStack[j]->suite == 3)
								heldCardStack[j]->cardBack.setFillColor(sf::Color::Red);
							else
								heldCardStack[j]->cardBack.setFillColor(sf::Color::White);

							heldCardStack[j]->x = (30.0f + (CARDWIDTH * 1.4) * (i + 1));
							heldCardStack[j]->y = (30.0f * (length + 1) + (j * 30));

							Location newLocation;
							newLocation.lastLocation = &cardPiles[i];
							newLocation.x = (30.0f + (CARDWIDTH * 1.4) * (i + 1));;
							newLocation.y = (30.0f * (length + 1) + (j * 30));
							heldCardStack[j]->currentLocation = newLocation;

							cardPiles[i].push_back(heldCardStack[j]);

							RemoveCardFromPreviousLocation(heldCardStack[j]);
						}

						heldCardStack.clear();
						return;
					}
				}
			}
		}

		ResetCardPosition();

		heldCardStack.clear();
	}
}

void Solitaire::MouseClicked(sf::RenderWindow& window, sf::Vector2f translatedPos, sf::RectangleShape cardStacks[7])
{
	heldCardStack.clear();

	sf::Time timeSinceLastClick = mouseClock.restart();

	if (undoText.getGlobalBounds().contains(translatedPos))
	{
		LoadFromHistory();
		return;
	}
	else if (restartText.getGlobalBounds().contains(translatedPos))
	{
		RestartGame();
		return;
	}

	//DRAWING DECK
	if (drawingDeck.size() > 0 && drawingDeck.back()->cardBack.getGlobalBounds().contains(translatedPos))
	{
		AddToHistory();

		for (int i = 0; i < 3; i++)
		{
			if (drawnCards[i] != NULL)
			{
				drawnCards[i]->x = 30;
				drawnCards[i]->y = 30;
				drawnCards[i]->bIsFaceDown = true;

				Location newLocation;
				newLocation.lastLocation = &drawingDeck;
				newLocation.x = 30;
				newLocation.y = 30;
				drawnCards[i]->currentLocation = newLocation;

				drawingDeck.insert(drawingDeck.begin(), drawnCards[i]);
			}
		}

		int length = fmin(3, drawingDeck.size());

		for (int i = 0; i < length; i++)
		{
			drawnCards[i] = drawingDeck.back();
			drawnCards[i]->x = 30;
			drawnCards[i]->y = (CARDHEIGHT * 1.3) + (30 * i);
			drawnCards[i]->bIsFaceDown = false;

			Location newLocation;
			newLocation.lastLocation = NULL;
			newLocation.x = 30;
			newLocation.y = (CARDHEIGHT * 1.3) + (30 * i);
			drawnCards[i]->currentLocation = newLocation;

			drawingDeck.pop_back();
		}

		return;
	}

	for (int i = 2; i >= 0; i--)
	{
		Card* card = drawnCards[i];
		if (card != NULL)
		{
			if (card->cardBack.getGlobalBounds().contains(translatedPos))
			{
				if (CheckIfDoubleClicked(card, timeSinceLastClick))
					return;

				heldCardStack.push_back(card);
			}

			break;
		}
	}

	//CARD PILES - 7
	for (int i = 0; i < 7; i++) // checks if player clicked on card in the stacks and sets it as the held card to move
	{
		int length = cardPiles[i].size();

		for (int j = length - 1; j >= 0; j--)
		{
			if (cardPiles[i][j]->bIsFaceDown)
				break;

			sf::RectangleShape cardBack = cardPiles[i][j]->cardBack;
			if (cardBack.getGlobalBounds().contains(translatedPos))
			{
				if (CheckIfDoubleClicked(cardPiles[i][j], timeSinceLastClick))
					return;

				for (int k = j; k < length; k++)
				{
					heldCardStack.push_back(cardPiles[i][k]);
				}

				return;
			}
		}
	}

	//ACE PILE - 4
	for (int i = 0; i < SUITESIZE; i++) // checks if player clicked on card in the stacks and sets it as the held card to move
	{
		int length = slot[i].size();

		for (int j = length - 1; j >= 0; j--)
		{
			if (slot[i][j]->bIsFaceDown)
				break;

			sf::RectangleShape cardBack = slot[i][j]->cardBack;
			if (cardBack.getGlobalBounds().contains(translatedPos))
			{
				if (CheckIfDoubleClicked(slot[i][j], timeSinceLastClick))
					return;

				for (int k = j; k < length; k++)
				{
					heldCardStack.push_back(slot[i][k]);
				}

				return;
			}
		}
	}
}

void Solitaire::RestartGame()
{
	lastClickedCard = nullptr;

	for (int i = 0; i < 3; i++)
	{
		drawnCards[i] = NULL;
	}

	for (int i = 0; i < 4; i++)
	{
		slot[i].clear();
	}

	for (int i = 0; i < 7; i++)
	{
		cardPiles[i].clear();
	}

	drawingDeck.clear();
	history.clear();
	heldCardStack.clear();
	mouseClock.restart();

	bIsGameOver = false;

	Initialize(pileBackgrounds, slotBackgrounds);
}

bool Solitaire::CheckIfDoubleClicked(Solitaire::Card* card, sf::Time timeSinceLastClick)
{
	if (card == lastClickedCard && timeSinceLastClick.asSeconds() < 0.3)
	{
		if ((slot[0].size() > 0 && (card->number == (slot[0].back()->number + 1) && card->suite == slot[0].back()->suite)) || (slot[0].size() == 0 && card->number == 0))
		{
			AddToHistory();

			card->x = slotBackgrounds[0].getPosition().x;
			card->y = slotBackgrounds[0].getPosition().y;

			Location newLocation;
			newLocation.lastLocation = &slot[0];
			newLocation.x = slotBackgrounds[0].getPosition().x;
			newLocation.y = slotBackgrounds[0].getPosition().y;
			card->currentLocation = newLocation;

			slot[0].push_back(card);
			RemoveCardFromPreviousLocation(card);
		}
		else if ((slot[1].size() > 0 && (card->number == (slot[1].back()->number + 1) && card->suite == slot[1].back()->suite)) || (slot[1].size() == 0 && card->number == 0))
		{
			AddToHistory();

			card->x = slotBackgrounds[1].getPosition().x;
			card->y = slotBackgrounds[1].getPosition().y;

			Location newLocation;
			newLocation.lastLocation = &slot[1];
			newLocation.x = slotBackgrounds[1].getPosition().x;
			newLocation.y = slotBackgrounds[1].getPosition().y;
			card->currentLocation = newLocation;

			slot[1].push_back(card);
			RemoveCardFromPreviousLocation(card);
			CheckForVictory();
		}
		else if ((slot[2].size() > 0 && (card->number == (slot[2].back()->number + 1) && card->suite == slot[2].back()->suite)) || (slot[2].size() == 0 && card->number == 0))
		{
			AddToHistory();

			card->x = slotBackgrounds[2].getPosition().x;
			card->y = slotBackgrounds[2].getPosition().y;

			Location newLocation;
			newLocation.lastLocation = &slot[2];
			newLocation.x = slotBackgrounds[2].getPosition().x;
			newLocation.y = slotBackgrounds[2].getPosition().y;
			card->currentLocation = newLocation;

			slot[2].push_back(card);
			RemoveCardFromPreviousLocation(card);
			CheckForVictory();
		}
		else if ((slot[3].size() > 0 && (card->number == (slot[3].back()->number + 1) && card->suite == slot[3].back()->suite)) || (slot[3].size() == 0 && card->number == 0))
		{
			AddToHistory();

			card->x = slotBackgrounds[3].getPosition().x;
			card->y = slotBackgrounds[3].getPosition().y;

			Location newLocation;
			newLocation.lastLocation = &slot[3];
			newLocation.x = slotBackgrounds[3].getPosition().x;
			newLocation.y = slotBackgrounds[3].getPosition().y;
			card->currentLocation = newLocation;

			slot[3].push_back(card);
			RemoveCardFromPreviousLocation(card);
			CheckForVictory();
		}

		return true;
	}

	lastClickedCard = card;
	return false;
}

void Solitaire::AddToHistory()
{
	GameState* currentState = new GameState();

	for (int i = 0; i < SUITESIZE; i++)
	{
		for (int j = 0; j < CARDSIZE; j++)
		{
			currentState->deck[i][j] = deck[i][j];
		}
	}

	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < cardPiles[i].size(); j++)
		{
			currentState->cardPiles[i].push_back(cardPiles[i][j]);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < slot[i].size(); j++)
		{
			currentState->slot[i].push_back(slot[i][j]);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		currentState->drawnCards[i] = drawnCards[i];
	}

	currentState->drawingDeck = drawingDeck;

	if (history.size() >= 10)
		history.erase(history.begin());

	history.push_back(*currentState);

	delete currentState;
}

void Solitaire::LoadFromHistory()
{
	if (history.size() == 0)
		return;

	const GameState* lastState = &history.back();

	for (int i = 0; i < 7; i++)
	{
		cardPiles[i].clear();

		for (int j = 0; j < lastState->cardPiles[i].size(); j++)
		{
			cardPiles[i].push_back(lastState->cardPiles[i][j]);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		slot[i].clear();

		for (int j = 0; j < lastState->slot[i].size(); j++)
		{
			slot[i].push_back(lastState->slot[i][j]);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		drawnCards[i] = lastState->drawnCards[i];
	}

	drawingDeck = lastState->drawingDeck;

	for (int i = 0; i < SUITESIZE; i++)
	{
		for (int j = 0; j < CARDSIZE; j++)
		{
			deck[i][j].x = lastState->deck[i][j].currentLocation.x;
			deck[i][j].y = lastState->deck[i][j].currentLocation.y;
			deck[i][j].bIsFaceDown = lastState->deck[i][j].bIsFaceDown;
			deck[i][j].cardBack = lastState->deck[i][j].cardBack;
			deck[i][j].currentLocation = lastState->deck[i][j].currentLocation;
		}
	}

	history.pop_back();
}

void Solitaire::ResetCardPosition()
{
	// Resets Card Back to its previous location
	for (int i = 0; i < heldCardStack.size(); i++)
	{
		if (heldCardStack[i] != NULL)
		{
			heldCardStack[i]->x = heldCardStack[i]->currentLocation.x;
			heldCardStack[i]->y = heldCardStack[i]->currentLocation.y;
		}
	}
}

void Solitaire::MoveToSlot(sf::RectangleShape cardSlot[4], int i)
{
	AddToHistory();

	if (heldCardStack[0]->suite == 1 || heldCardStack[0]->suite == 3)
		heldCardStack[0]->cardBack.setFillColor(sf::Color::Red);
	else
		heldCardStack[0]->cardBack.setFillColor(sf::Color::White);

	heldCardStack[0]->x = cardSlot[i].getPosition().x;
	heldCardStack[0]->y = cardSlot[i].getPosition().y;

	//TODO - SET last location top card to face up

	Location newLocation;
	newLocation.lastLocation = &slot[i];
	newLocation.x = cardSlot[i].getPosition().x;
	newLocation.y = cardSlot[i].getPosition().y;
	heldCardStack[0]->currentLocation = newLocation;

	slot[i].push_back(heldCardStack[0]);

	RemoveCardFromPreviousLocation(heldCardStack[0]);

	heldCardStack.clear();

	CheckForVictory();
}

void Solitaire::CheckForVictory()
{
	int lengthA = slot[0].size();
	int lengthB = slot[1].size();
	int lengthC = slot[2].size();
	int lengthD = slot[3].size();

	if (lengthA == 13 && lengthB == 13 && lengthC == 13 && lengthD == 13)
	{
		bIsGameOver = true;
	}
}

void Solitaire::RemoveCardFromPreviousLocation(Solitaire::Card* cardToRemove)
{
	//Checks if card was in drawing deck and removes it
	auto deckIt = std::find_if(drawingDeck.begin(), drawingDeck.end(), [&](Card* card) { return card == cardToRemove;  });

	if (deckIt != drawingDeck.end())
	{
		drawingDeck.erase(deckIt);
	}

	//Checks if card was in one of the card piles and removes it
	for (int i = 0; i < 7; i++)
	{
		//Checks if the card was moved to this pile and skips it
		if (cardToRemove->currentLocation.lastLocation == &cardPiles[i])
			continue;

		auto it = std::find_if(cardPiles[i].begin(), cardPiles[i].end(), [&](Card* card) { return card == cardToRemove;  });

		if (it != cardPiles[i].end())
		{
			cardPiles[i].erase(it);
		}
	}

	//Checks if card was in one of the Ace Stacks and removes it
	for (int i = 0; i < SUITESIZE; i++)
	{
		int length = slot[i].size();

		//Checks if the card was moved to this stack and skips it
		if (cardToRemove->currentLocation.lastLocation == &slot[i] || length == 0)
			continue;

		if (cardToRemove == slot[i][length - 1])
		{
			slot[i].erase(slot[i].begin() + (length - 1));
		}
	}

	//Checks if card was one of the three cards drawn and removes it
	for (int i = 0; i < 3; i++)
	{
		if (drawnCards[i] == cardToRemove)
		{
			drawnCards[i] = nullptr;
		}
	}
}

void Solitaire::HandleCardGraphics(Card* card, sf::RenderWindow& window)
{
	if (card->bIsFaceDown == false)
	{
		sf::Texture suiteTexture;
		suiteTexture.loadFromFile("Images/suites.png");

		sf::Sprite suiteSprite(suiteTexture);

		std::string letter = " ";
		switch (card->number)
		{
		case 12:
			letter = 'K';
			break;
		case 11:
			letter = 'Q';
			break;
		case 10:
			letter = 'J';
			break;
		case 0:
			letter = 'A';
			break;
		default:
			letter = std::to_string(card->number + 1);
			break;
		}

		int textWidth = 50;
		int textHeight = 49;

		std::string suite = " ";
		switch (card->suite)
		{
		case 0:
			suite = "S";
			suiteSprite.setTextureRect(sf::IntRect(0, textHeight * 3, textWidth, textHeight));
			break;
		case 1:
			suite = "D";
			suiteSprite.setTextureRect(sf::IntRect(0, 0, textWidth, textHeight + 2));
			break;
		case 2:
			suite = "C";
			suiteSprite.setTextureRect(sf::IntRect(0, textHeight + 2, textWidth, textHeight));
			break;
		case 3:
			suite = "H";
			suiteSprite.setTextureRect(sf::IntRect(0, textHeight * 2, textWidth, textHeight));
			break;
		default:
			break;
		}

		DrawUIText(window, letter, sf::Vector2f(card->x + 5, card->y), 20, 0);
		//DrawUIText(window, suite, sf::Vector2f(card->x + (CARDWIDTH - 20), card->y), 20, 0);

		suiteSprite.setPosition(card->x + (CARDWIDTH / 2) - (suiteSprite.getGlobalBounds().width / 2), card->y + (CARDHEIGHT / 2) - (suiteSprite.getGlobalBounds().height / 2));
		window.draw(suiteSprite);
	}
}

void Solitaire::DrawUIText(sf::RenderWindow& window, const std::string textString, sf::Vector2f textPos, int textSize, int drawDirection = 0)
{
	sf::Font font;
	if (font.loadFromFile("Fonts/LiberationSans.ttf"))
	{
		sf::Text text;
		// select the font
		text.setFont(font); // font is a sf::Font
		// set the string to display
		text.setString(textString);
		// set the character size
		text.setCharacterSize(textSize); // in pixels, not points!
		// set the color
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1);
		// set the text style
		text.setStyle(sf::Text::Bold);
		// set pos
		sf::FloatRect boundingBox = text.getGlobalBounds();

		switch (drawDirection)
		{
		case 1: //right
		{
			text.setOrigin(boundingBox.width, 0);
			break;
		}
		case 2: //center
		{
			text.setOrigin(boundingBox.width / 2, 0);
			break;
		}
		default: // left
		{
			break;
		}
		}

		text.setPosition(textPos.x, textPos.y);
		// inside the main loop, between window.clear() and window.display()
		window.draw(text);
	}
}