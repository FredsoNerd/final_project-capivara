from matplotlib import pyplot as plt
import capivara as cp

# from dictiony
df = cp.DataFrame({
    'City_Code': [1, 2, 3, 4, 5],
    'Latitude': [-34.58, -15.78, -33.45, 4.60, 10.48],
    'Longitude': [-58.66, -47.91, -70.66, -74.08, -66.86]
})
# df.set_column("geometry", cp.points_from_xy(df["Longitude"], df["Latitude"]))
df["geometry"] = cp.points_from_xy(df["Longitude"], df["Latitude"])

# print(df)
df.show()
tabl = df.tabulate(tablefmt="github"); print(tabl)

# dispersion plot
df.plot('Latitude', 'Longitude', color='b')
plt.plot(df['Latitude'], df['Longitude']); plt.show()

# Use uma estrutura 1D para indexar os dados
df.set_index("City_Code")
df.equal(5).show()
df.between(0, 10).show()
df.between(-0.5, 4).show()

# Use uma estrutura 2D para indexar os pontos
df.set_index("geometry")
query_rect = [0, 10, -1, 20]
df.intersection(query_rect).show()
print(df.intersection([-100,100,-100,100]))
