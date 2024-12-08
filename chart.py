import matplotlib.pyplot as plt

# Function to read data from the text file
def read_data(filename):
    labels = []
    sizes = []
    with open(filename, 'r') as file:
        for line in file:
            # Split the line by comma, strip any extra spaces, and process
            label, size = line.strip().split(',')
            labels.append(label)
            sizes.append(int(size))
    return labels, sizes

# File containing the data (update with your file path)
filename = "C:\\Users\\KNUser\\Desktop\\integration\\integration\\data.txt"

# Read the data
labels, sizes = read_data(filename)

# Optional: define custom colors
colors = ['gold', 'yellowgreen', 'lightcoral', 'lightskyblue']  # Customize as needed

# Create the pie chart\
plt.figure(figsize=(6,6))
plt.pie(sizes, labels=labels, colors=colors[:len(labels)], autopct='%1.1f%%', startangle=140)

# Equal aspect ratio ensures that pie is drawn as a circle.
plt.axis('equal')

# Save the chart as a PNG file
plt.savefig("C:\\Users\\KNUser\\Desktop\\integration\\integration\\pie_chart.png", format='png',dpi=200,bbox_inches='tight')

# Optionally, show the plot
# plt.show()
