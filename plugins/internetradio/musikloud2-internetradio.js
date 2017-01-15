/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

var API_URL = "http://marxoft.co.uk/api/cuteradio";
var COUNTRIES_URL = API_URL + "/countries";
var GENRES_URL = API_URL + "/genres";
var LANGUAGES_URL = API_URL + "/languages";
var STATIONS_URL = API_URL + "/stations";

var PLAYLIST_EXTS = ["asx", "m3u", "pls", "ram", "smil"];

var MAX_RETRIES = 8;

var USER_AGENT = "Wget/1.13.4 (linux-gnu)";

var request = null;
var retries = 0;

function getPlaylistExt(url) {
    var ext = url.substring(url.lastIndexOf(".") + 1).toLowerCase();
    
    if (PLAYLIST_EXTS.indexOf(ext) !== -1) {
        return ext;
    }
    
    return null;
}

function urlIsPlaylist(url) {
    return getPlaylistExt(url) !== null;
}

function getStreamUrlFromUnknownFile(response) {
    try {
        return /(http(s|)|mms):\/\/[^\s"<>]+/.exec(response)[0];
    }
    catch(err) {
        return null;
    }
}

function getStreamUrlFromASXFile(response) {
    return getStreamUrlFromUnknownFile(response);
}

function getStreamUrlFromM3UFile(response) {
    try {
        return /(http(s|)|mms):\/\/\S+/.exec(response)[0];
    }
    catch(err) {
        return null;
    }
}

function getStreamUrlFromPLSFile(response) {
    try {
        return /(File\d=)(\S+)/.exec(response)[1];
    }
    catch(err) {
        return null;
    }
}

function getStreamUrlFromRAMFile(response) {
    return getStreamUrlFromUnknownFile(response)
}

function getStreamUrlFromSMILFile(response) {
    return getStreamUrlFromUnknownFile(response)
}

function listStations(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var result = JSON.parse(request.responseText);
                
                for (var i = 0; i < result.items.length; i++) {
                    result.items[i]["downloadable"] = false;
                    result.items[i]["id"] = result.items[i].source;
                    result.items[i]["relatedTracksId"] = STATIONS_URL + "?genre=" + result.items[i].genre;
                    result.items[i]["url"] = result.items[i].source;
                    
                    if (!urlIsPlaylist(result.items[i].source)) {
                        result.items[i]["streamUrl"] = result.items[i].source;
                    }
                }
                
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function searchStations(query, order) {
    listStations(STATIONS_URL + "?search=" + query + "&sort=" + order);
}

function listCategories(url) {
    var category = "genres";
    
    try {
        var path = url.split("?")[0];
        category = path.substring(path.lastIndexOf("/") + 1);
    }
    catch(err) {}
    
    switch (category) {
    case "genres":
        listGenres(url);
        break;
    case "countries":
        listCountries(url);
        break;
    case "languages":
        listLanguages(url);
        break;
    default:
        error("Invalid category type");
        break;
    }
}

function listGenres(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var result = JSON.parse(request.responseText);
                
                for (var i = 0; i < result.items.length; i++) {
                    result.items[i]["title"] = result.items[i].name;
                    result.items[i]["tracksId"] = STATIONS_URL + "?genre=" + result.items[i].name;
                }
                
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listCountries(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var result = JSON.parse(request.responseText);
                
                for (var i = 0; i < result.items.length; i++) {
                    result.items[i]["title"] = result.items[i].name;
                    result.items[i]["tracksId"] = STATIONS_URL + "?country=" + result.items[i].name;
                }
                
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listLanguages(url) {
    if (url.indexOf("/") === 0) {
        url = API_URL + url;
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {
            try {
                var result = JSON.parse(request.responseText);
                
                for (var i = 0; i < result.items.length; i++) {
                    result.items[i]["title"] = result.items[i].name;
                    result.items[i]["tracksId"] = STATIONS_URL + "?language=" + result.items[i].name;
                }
                
                finished(result);
            }
            catch(err) {
                error(err);
            }
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function listStreams(url) {
    var ext = getPlaylistExt(url);
    
    if (!ext) {
        retries = 0;
        finished({"items": [{"description": "Live radio stream", "id": "live", "url": url}]});
        return;
    }
    
    if (retries >= MAX_RETRIES) {
        retries = 0;
        error("Maximum retries reached");
        return;
    }
    
    request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (request.readyState === 4) {            
            switch (ext) {
            case "asx":
                url = getStreamUrlFromASXFile(request.responseText);
                break;
            case "m3u":
                url = getStreamUrlFromM3UFile(request.responseText);
                break;
            case "pls":
                url = getStreamUrlFromPLSFile(request.responseText);
                break;
            case "ram":
                url = getStreamUrlFromRAMFile(request.responseText);
                break;
            case "smil":
                url = getStreamUrlFromSMILFile(request.responseText);
                break;
            default:
                url = getStreamUrlFromUnknownFile(request.responseText);
                break;
            }
            
            ++retries;
            listStreams(url);
        }
    }
    
    request.open("GET", url);
    request.setRequestHeader("User-Agent", USER_AGENT);
    request.send();
}

function list(resource, id) {
    switch (resource) {
    case "track":
        listStations(id);
        return true;
    case "category":
        listCategories(id);
        return true;
    case "stream":
        listStreams(id);
        return true;
    default:
        return false;
    }
}

function search(resource, query, order) {
    switch (resource) {
    case "track":
        searchStations(query, order);
        return true;
    default:
        return false;
    }
}

function cancel() {
    if (request) {
        request.abort();
    }
    
    return true;
}
