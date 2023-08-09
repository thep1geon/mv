import os

def main():
    for _, _, files, in os.walk("examples/"):
        for name in files:
            if name.endswith(".mv"):
                print(name)
                print("Output: ")
                os.system(f"./run examples/{name}")
                print("----------------------------------------------------")
                print()

if __name__ == "__main__":
    main()
