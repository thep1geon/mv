import os

def main():
    os.system("./run")
    for _, _, files, in os.walk("examples/"):
        for name in files:
            if name.endswith(".mv"):
                print(name)
                print("Output: ")
                os.system(f"./mv examples/{name}")
                print("----------------------------------------------------")
                print()

if __name__ == "__main__":
    main()
