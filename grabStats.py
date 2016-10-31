from __future__ import print_function
import httplib2
import os
from datetime import datetime
import pytz

from apiclient import discovery
from oauth2client import client
from oauth2client import tools
from oauth2client.file import Storage

try:
    import argparse
    flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
except ImportError:
    flags = None

# If modifying these scopes, delete your previously saved credentials
# at ~/.credentials/sheets.googleapis.com-python-quickstart.json
SCOPES = 'https://www.googleapis.com/auth/spreadsheets'
CLIENT_SECRET_FILE = 'client_secret.json'
APPLICATION_NAME = 'Google Sheets API Python Quickstart'

spreadsheetId = '1zfJTTZJsjKOBelItdlcavzVYIfrRArD-GHXPlqbiXVM'


def get_credentials():
    """Gets valid user credentials from storage.

    If nothing has been stored, or if the stored credentials are invalid,
    the OAuth2 flow is completed to obtain the new credentials.

    Returns:
        Credentials, the obtained credential.
    """
    home_dir = os.path.expanduser('~')
    credential_dir = os.path.join(home_dir, '.credentials')
    if not os.path.exists(credential_dir):
        os.makedirs(credential_dir)
    credential_path = os.path.join(credential_dir,
                                   'sheets.googleapis.com-python-quickstart.json')

    store = Storage(credential_path)
    credentials = store.get()
    if not credentials or credentials.invalid:
        flow = client.flow_from_clientsecrets(CLIENT_SECRET_FILE, SCOPES)
        flow.user_agent = APPLICATION_NAME
        if flags:
            credentials = tools.run_flow(flow, store, flags)
        else: # Needed only for compatibility with Python 2.6
            credentials = tools.run(flow, store)
        print('Storing credentials to ' + credential_path)
    return credentials

def input_value(value, label, position, service, mode = 0):
    if not value:
        if mode == 0:
            rvalue = float(raw_input(label))
        elif mode == 1:
            rvalue = 'yes' if (raw_input(label)) else 'no'
        elif mode == 2:
            tmp = raw_input(label)
            rvalue = tmp if tmp else 'Europe/London'
        body = {'values': [[rvalue]]}
        result = service.spreadsheets().values().update(spreadsheetId=spreadsheetId, range=position,
                                                        valueInputOption="USER_ENTERED", body=body).execute()
        return rvalue
    else:
        if mode == 0:
            return float(value)
        elif mode in [1, 2]:
            return str(value)

def icol_to_acol(icol):
    return chr(icol + ord('A'))

def main():
    """Shows basic usage of the Sheets API.

    Creates a Sheets API service object and prints the names and majors of
    students in a sample spreadsheet:
    https://docs.google.com/spreadsheets/d/1BxiMVs0XRA5nFMdKvBdBZjgmUUqptlbs74OgvE2upms/edit
    """
    credentials = get_credentials()
    http = credentials.authorize(httplib2.Http())
    discoveryUrl = ('https://sheets.googleapis.com/$discovery/rest?'
                    'version=v4')
    service = discovery.build('sheets', 'v4', http=http,
                              discoveryServiceUrl=discoveryUrl)

    rangeName = 'Sheet1!A2:J'
    result = service.spreadsheets().values().get(
        spreadsheetId=spreadsheetId, range=rangeName).execute()
    values = result.get('values', [])

    if not values:
        print('No data found.')
    else:
        print('Start time, Distance, Speed, Elevation, Commute:')
        f = open('toms_bike_ride.txt', 'w')
        utc = pytz.utc
        london = pytz.timezone('Europe/London')
        for irow, row in enumerate(values):
            # Print columns A, C, H, I, J, which correspond to indices 0, 2, ...
            date = datetime.strptime(row[0], '%B %d, %Y at %I:%M%p')
            distance = float(row[2]) / 1609.3439 # metres to miles
            while(len(row)) < 11:
                row.append(None)
            speed = input_value(row[7], str(date) + ' Speed (mph): ', 'Sheet1!' + icol_to_acol(7) + str(irow + 2), service)
            elevation = input_value(row[8], str(date) + ' Elevation gain (ft): ', 'Sheet1!' + icol_to_acol(8) + str(irow + 2), service)
            commute = input_value(row[9], str(date) + ' Commute (default: no): ', 'Sheet1!' + icol_to_acol(9) + str(irow + 2), service, 1)
            timezone = input_value(row[10], str(date) + ' Timezone (default: Europe/London): ', 'Sheet1!' + icol_to_acol(10) + str(irow + 2), service, 2)
            local_date = pytz.timezone(timezone).localize(date)
            utc_date = local_date.astimezone(utc)
            print("%s, %.1f miles, %.1f mph, %.1f ft%s" % (utc_date, distance, speed, elevation, ', commute' if commute == 'yes' else ''))
            f.write('%s %.1f %.1f %d %s\n' % (utc_date.strftime('%s'), distance, speed, elevation, '1' if commute == 'yes' else '0'))
        f.close()

if __name__ == '__main__':
    main()
