import p2
import p3
import p6
def number_to_string(argument):
    if(argument == 1):
        p2.run()
    if(argument == 2):
        p3.run()
    if(argument == 3):
        p6.run()
    
 
 
if __name__ == "__main__":
    print("press 1 for part a\npress 2 for part b\npress 3 for part d\n")
    argument = int(input())
    if(argument <1 or argument>3):
        print("invalid option")
    else:
        number_to_string(argument)
