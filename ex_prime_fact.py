def prime_factorization(num):
  print(f"{num} = ", end="")
  i=2
  while(i<=num):
    if(num==i):
      print(f"{i}\n", end="")
      return 
      
    if(num%i==0):
      print(f"{i} * ", end="")
      num/=i
      
    else:
      i+=1
      
    
  
if __name__ == "__main__":
  prime_factorization(462)
  
