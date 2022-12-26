def factorial(n):
  if(n==0):
    return 1
    
  return n*factorial(n-1)
  
if __name__ == "__main__":
  x=5
  x_fact=factorial(5)
  print(f"{x}! = {x_fact}\n", end="")
  
