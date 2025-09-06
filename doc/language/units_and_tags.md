# Units and tags

## Usage

### Literals

Units can be used with literals. You may directly add the name of a unit
or if it is unit-expression you may add those between parentheses.

```
let foo = 1.0 m
let bar = 100.0 cm
let baz = 120.0 (km/h)
```

The type of these literals is:
 - The base type, in the above case `real`, or a floating point number.
 - A unit tag, which consists of:
   * The realm, like: si
   * A quantity expression condenced to a vector of exponentions.

## Declarations

### Realms
A realm contains a set of units and constants. The most well know realm is
the "SI International System of Units". But you can imagine units like pixels
that do not fall within this realm.

So there are multiple realms of interconnected units:
 - #si : SI International System of Units
 - #font : Em
 - #screen : pixel

```
declare realm si
declare realm font
declare realm screen
```

### Base Quantities

Here we declare the base quantities, each quantity may exist in different realms.
You can specify the default realm so that there is a short-cut when adding a
quantity to a type.

```
declare quantity length default si
declare quantity mass default si
declare quantity time default si
declare quantity electric_current default si
declare quantity thermodynamic_temperature default si
declare quantity amount_of_substance default si
declare quantity luminous_intensity default si
```

### Derived Quantities

```
declare quantity frequency = time^-1
declare quantity force = length * mass * time^-2
declare quantity presure = length^-1 * mass * time^-2
declare quantity velocity = length * time^-1
declare quantity area = length^2
declare quantity volume = length^3
declare quantity acceleration = length * time^-2
declare quantity width = length
declare quantity height = length
declare quantity electrical_resistance = length^2 * mass * time^-3 * electric_current^-2
```


### Base unit
A base unit is a standard quantity in a realm. For example `m` (meter) is the
base unit `1.0` of the `length` quantity in the `si` realm.

The declaration of a unit includes the value, realm and quantity. This will
add the base-quantity(s) to the exponent-vector of the realm.

```
declare unit px = 1.0 screen length
declare unit Em = 1.0 font length
declare unit m = 1.0 si length
declare unit m2 "m²" = 1.0 si area
declare unit Hz = 1.0 si frequency
declare unit ohm "Ω" = 1.0 si electrical_resistance
```

### Scale unit
There are also scaled units.

```
declare unit km = 1000 * m
declare unit cm = 0.01 * m
declare unit mm = 0.001 * m
declare unit in "″" = 2.54 * cm
declare unit ft "′" = 12 * in
declare unit C "°C" = K - 273.15
declare unit F "°F" = (9/5)*C + 32
```


