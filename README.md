# airsoft_bomb (in development)
---
A airsoft bomb project, to be defused through wire cutting.

There's a lot of things to do.

## About thr circuit diagram

Made it simple, no big deal.

![circuit-diagram](/images/diagram.jpeg)

Sadly, **the Arduino Uno doesn't have enough ports/slots/pins do make the wire-cutting way to defuse the bomb in fair way of course.** For the unfair way, you could just cut the energy wire and then no one will now if you defused in time.

To workaround this issue, **you can use another Arduino platform or another controller that adds few more slots and ajust it on code.** 

## Project current situation

For this development, I was using an [arduino mega 2560](https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.embarcados.com.br%2Farduino-mega-2560%2F&psig=AOvVaw02J97F3FZZOOFx3KxrEvSM&ust=1647964911499000&source=images&cd=vfe&ved=0CAsQjRxqFwoTCLDZ4erJ1_YCFQAAAAAdAAAAABAG) wich got a lot of ports to work (but later found that many of they were not working properly).

In the next few days, I'm going to make some tests with the [arduino nano](https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.robocore.net%2Fplaca-arduino%2Fplaca-nano-v3-com-cabo-usb-para-arduino&psig=AOvVaw2JEEym1v1zYe1reBIJkYZS&ust=1647964989668000&source=images&cd=vfe&ved=0CAsQjRxqFwoTCNinl5DK1_YCFQAAAAAdAAAAABAF) wich seens to have one or two more ports/slots/pins than the arduino uno.

And for replication and market availability, some components used will have to change and so the code.

## TODO:
- Components list
- 3D case printing model and configuration
- Assemble guide
- Test cases
- Project cost
- Requirements
- (to be defined)
