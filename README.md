# Commodore 64 connection to OpenAI API via an Esp8266

## Configuration

Copy the `/include/config.dist.h` to `/include/config.h` and fill it out with your data.

Copy the `/platformio.dist.ini` to `/platformio.ini` and fill it out with your data.

## Commodore 64 Program

```
10 POKE 53280,0
11 POKE 53281,0
20 PRINT "{CLEAR}{WHITE}"
21 PRINT CHR#(159)
22 PRINT"STELLE C64-GPT DEINE FRAGE. WARTE DANACH EINEIGE SEKUNDEN."
30 OPEN2,2,4,CHR$(8)+CHR$(0)
40 GET#2,S$
200 GET K$
201 POKE 204,0
220 IF K$<>""THEN PRINT#2,K$;
230 GET#2,S$
240 PRINT S$;
245 IF (PEEK(673)AND1)THEN 245
900 GOTO 200
1000 CLOSE 2:END
```

## Resources

- [Retro Game Coders — Commodore 64 + Arduino (C64 Electronics Part 4)](https://retrogamecoders.com/commodore-64-arduino/)
- [Commodore 64 Programmer's Reference Guide](https://archive.org/details/c64-programmer-ref/page/n377/mode/2up)
- [C64 Wiki — PETSCII Codes in Listings](https://www.c64-wiki.com/wiki/PETSCII_Codes_in_Listings)
- [C64 Wiki — RS-232](https://www.c64-wiki.com/wiki/RS-232)
- [Awesome ChatGPT Prompts](https://github.com/f/awesome-chatgpt-prompts)
- [OpenAI API Reference](https://platform.openai.com/docs/api-reference/chat/create)
