# MIRAKEL MONITOR 2.0
MirakelMonitor 2.0: een monitor voor CCOL regelingen. De monitor maakt interne data uit de CCOL applicatie op miraculeuze wijze zichtbaar.

# Gebruik
Om de monitor te gebruiken moet deze eerst worden gebouwd:
- Clone (of download) het project lokaal
- Bouw met Visual 2017 (Merk op: Visual moet zijn geïnstalleerd met de mogelijkheid voor compileren van C en Win32)
Vervolgens kan de monitor in een CCOL regeling worden gebruikt:
- Neem `MirakelMonitor.h` op in de reg.add (of een vergelijkbaar bestand):
```c
#include "MirakelMonitor.h"
```
- Gebruik de volgende code om de monitor te initialiseren, bv. onder `post_init_application()`:
```c
if (!SAPPLPROG) MirakelMonitor_init(SYSTEM);
```
- De monitor moet elke systeemronde worden geupdate, bijvoorbeeld in `SpecialSignals()` zodat het ook wordt ververst tijdens halteren in de testomgeving:
```c
MirakelMonitor();
```
