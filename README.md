# placeid-finder
OSM Place Search Tool (Qt)
A lightweight Qt desktop application that provides fast location search using the OpenStreetMap Nominatim API. Users can search any place and view results in a structured table, including OSM ID, place name, type, and full address. The OSM ID can be copied instantly by double-clicking the table cell. No API key required.

Users can quickly look up:

    OSM ID
    Place Name
    Type
    Full Address
    Double-clicking the OSM ID automatically copies it to clipboard.


Features

    Search Places: Enter any place
    Uses OpenStreetMap (Nominatim API), No API key required, Free, open-source
    Results in Table Format: OSM ID, Name, Type, Address
    Copy OSM ID: Double-click the OSM ID cell, automaticall will copied to clipboard.


Technologies Used

    Qt 6
    Qt Widgets
    QNetworkAccessManager
    Nominatim (OpenStreetMap) Search API
    JSON parsing (QJsonDocument, QJsonArray)
    
