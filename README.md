Leo King lking1 
A laboratory with Neo Agojo Iagojo@ucsc.edu
My repo contains the final version of the project. 
I wrote:
- Message.c 
- Negotiation.c 
- AgentTest.c 
- FieldTest.c 
- A portion of Field.c and MessageTest.c
We didn't do any extra credit.

Collaboration:
I collaborated with Neo. We worked together to build a BattleBoats game.

Summary of BattleBoats System:
Negotiation helps decide on who goes first depending on a flip of Heads and Tails. It is also used to check if
one side cheated on the coin flip. Message takes information and checks for errors that may have happened in
transmission. It uses the hash of the message and a checksum to make sure nothing is changed or got corrupted.
Message also decode and encodes messages that will be stored in a structure that will be accessed by the Agent.
The field is used to get/set status of squares using the field structure. It also sets boats and decides whether
a location was hit or missed. It stores the information in an array that holds each squares information. Agent
switches states to decide what the players are allowed to do. 

Efficiency and Testing Stategy:
I think writing tests really helped in understanding what needs to be outputted. The libraries helped a lot 
because I could use the libraries to first write functional tests that I can use in my own sections. When I 
get stuck I couldn't find the why I'm stuck from just the autograder so I end up be on something for multiple
days. 

Learned: 
I learned to use checksums and how hashes function. I read up on some of the lecture notes and everything was
described there. Checksums are basically to ensure that there isn't errors in transmission and hashes are can
be used to make checksums easy to calculate and difficult to reverse. 

Lab Conclusion:
I really enjoyed this lab because it seemed really cool to build a game. I wish the battle boats we made was 
functional. If I had a bit more time it would've been great to get it running all by myself so I can more
thoroughly understand how to build a game alone. I also liked working with another person because they help
give some perspective where you might not be able to see while doing a lab alone. If I were to change this lab
I think I would skip the lab that was due the week before with building a RPG and make this the final lab. 
That way we can learn comprehend what we're doing better or make this lab a mix of the two.
