# asuna-gmod
Asuna is [gluasteal](https://github.com/lewisclark/glua-steal) and [GMOD-SDK](https://github.com/Gaztoof/GMod-SDK) combined together, GMOD-SDK being as base and gluasteal as source for ScriptHook.

It works only on `x64` since `x86` is getting updates very lately and slowly, and it's just... feels outdated?

## Usage
- Compile `x64` in `Release` *(or download from releases)*
- Inject `asuna-gmod.dll` using any injector *([recommendation](https://processhacker.sourceforge.io/))*
- Press `INSERT` to open the menu.

## Contribution
- Fork the [project](https://github.com/shockpast/asuna-gmod)
- Create **new** branch for your new features: `git checkout -b feature/fooBar`
- Commit your changes: `git commit -am 'feat: add some fooBar'`
- Push it to your branch: `git push origin feature/fooBar`
- Create a new Pull Request containing features from your branch!

## Credits
- [GMOD-SDK](https://github.com/Gaztoof/GMod-SDK) for headers, most of the implementations and as whole for the good base
- [gluasteal](https://github.com/lewisclark/glua-steal) for ScriptHook implementations