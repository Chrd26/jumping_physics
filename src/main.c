#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "app.h"

int main(void)
{
    if (!InitApp())
    {
        exit(EXIT_FAILURE);
    }

    char *miniApplicationLocationCopy = calloc(strlen(miniApplication.location), sizeof(char));
    strncpy(miniApplicationLocationCopy, miniApplication.location, strlen(miniApplication.location));
    double startTick = 0, endTick = 0, frameTime = 0;
    char *temp;
    size_t currentStringSize = 0;
    Uint32 mouseState;
    bool quit = false;
    SDL_Event events;
    char *testCommand = "test -f ";
    char *fileLocation = "app/output/calculations.csv";
    char *createCommand = calloc(strlen(testCommand) + strlen(miniApplication.location) + strlen(fileLocation), 
                                 sizeof(char));
    strcpy(createCommand, testCommand);
    strcat(createCommand, miniApplication.location);
    strcat(createCommand, fileLocation);
    int getCalcFile = system(createCommand);

    free(createCommand);

    if (getCalcFile == 0)
    {
        char *cdCommand = "cd ";
        char *removeCommand = " && rm calculations.csv";
        createCommand = calloc( strlen(miniApplication.location) + strlen(cdCommand) + strlen(removeCommand), 
                               sizeof(char));
        getCalcFile = system(createCommand);
        free(createCommand);

        if (getCalcFile != 0)
        {
            printf("Calculations CSV removal failed\n, %d", errno);
            exit(EXIT_FAILURE);
        }
    }

    while(!quit)
    {
        // Start counting
        startTick = SDL_GetTicks();
        SDL_RenderClear(appRenderer);	
        currentStringSize = strlen(interiorTextBox.content);

        while(SDL_PollEvent(&events))
        {
            switch(events.type)
            {
                case SDL_EVENT_QUIT:
                    quit = true;
                break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (mouseState == LEFT_MOUSE_BUTTON)
                    {
                        if (IsHoveringExteriorBox(mouseX, mouseY, exteriorTextBox))
                        {
                            if(!interiorTextBox.isEnabled)
                            {
                                interiorTextBox.isEnabled = true;
                                SDL_StartTextInput();
                            }
                            break;
                        }

                        if (IsHoveringButton(mouseX, mouseY, startButton))
                        {
                            executionResults = GetResults(miniApplicationLocationCopy, interiorTextBox.content);
                            printf("Maximum Height, %f\n", executionResults.maxHeight);
                            printf("Time to maximum height, %f\n", executionResults.timeToMaximumHeight);
                            printf("Time to land, %f\n", executionResults.timeToLand);
                        }

                        if (IsHoveringButton(mouseX, mouseY, exitButton))
                        {
                            quit = true;
                        }

                        interiorTextBox.isEnabled = false;
                    SDL_StopTextInput();
                    }
                break;
                case SDL_EVENT_TEXT_INPUT:
                    if (currentStringSize >= 3)
                    {
                        break;
                    }

                    int getVal = (int)*events.text.text;

                    if (getVal - 48 < 0 || getVal - 48 > 9)
                    {
                        break;
                    }
                    temp = calloc(currentStringSize + 1, sizeof(char));
                    strncpy(temp, interiorTextBox.content, currentStringSize);
                    strncat(temp, events.text.text, 1);
                    free(interiorTextBox.content);
                    interiorTextBox.content = calloc(strlen(temp), sizeof(char));
                    strncpy(interiorTextBox.content, temp, strlen(temp));
                    free(temp);
                break;
                case SDL_EVENT_KEY_UP:
                    switch(events.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            interiorTextBox.isEnabled = false;
                            SDL_StopTextInput();
                        break;
                        case SDLK_BACKSPACE:
                            if (currentStringSize <= 0)
                            {
                                break;
                            }
                            temp = calloc(currentStringSize - 1, sizeof(char));
                            strncpy(temp, interiorTextBox.content, currentStringSize - 1);
                            free(interiorTextBox.content);
                            interiorTextBox.content = calloc(strlen(temp), sizeof(char));
                            strncpy(interiorTextBox.content, temp, strlen(temp));
                            free(temp);
                        break;
                    }
                break;
            }
        }

        SDL_PumpEvents();
        mouseState = SDL_GetMouseState(&mouseX, &mouseY);

        DisplayText(appRenderer, appFont, "Add a starting value and press start", 
                    windowWidth * 0.065, windowHeight * 0.1);	
        SDL_SetRenderDrawColor(	appRenderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderLine(appRenderer, windowWidth/2, 0, windowWidth/2,  windowHeight);

        TextBoxHandler(	textboxFont, appRenderer, exteriorTextBox, interiorTextBox, 
                       strlen(interiorTextBox.content), &frameTime);

        GenerateBall(ball, appRenderer);

        if ((mouseState &SDL_BUTTON_LMASK) != 0 && IsHoveringButton(mouseX, mouseY, startButton))
        {
            GenerateButton(appFont, appRenderer, &startButton, false);
        }
        else
        {
            GenerateButton(appFont, appRenderer, &startButton, IsHoveringButton(mouseX, mouseY, startButton));
        }


        if ((mouseState &SDL_BUTTON_LMASK) != 0 && IsHoveringButton(mouseX, mouseY, exitButton))
        {
            GenerateButton(appFont, appRenderer, &exitButton, false);
        }
        else
        {
            GenerateButton(appFont, appRenderer, &exitButton, IsHoveringButton(mouseX, mouseY, exitButton));
        }

        SDL_SetRenderDrawColor(	appRenderer, 0xAA, 0xAA, 0xFF, 0xFF);	
        SDL_RenderPresent(appRenderer);										

        endTick = SDL_GetTicks();
        // Get Time in Seconds
        frameTime += (endTick-startTick)/1000;
    }

    free(miniApplicationLocationCopy);
    ExitApplication();
    return EXIT_SUCCESS;
}

